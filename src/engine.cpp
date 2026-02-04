#include "../include/engine.h"
#include "../include/swarm.hpp"
#include "Perun/Graphics/Renderer.h"
#include "Perun/Graphics/Shader.h" // Explicit
#include <iostream>

// Swarm is used in main, but maybe Engine should own it?
// The original main had Swarm swarm; loop { swarm.render(); engine.draw(); }
// We can keep that structure or move Swarm inside.
// For now, let's keep Swarm in main but notice that main.cpp calls swarm.render().
// Engine.cpp implementation:

Engine::Engine()
{
  window = std::make_unique<Perun::Core::Window>("ParticleExplosion", 1280, 750);
  if (!window->Init()) {
      std::cerr << "Failed to init window" << std::endl;
      running = false;
      return;
  }
  
  Perun::Renderer::Init();
  
  // Setup Framebuffer
  Perun::Graphics::FramebufferSpecification spec;
  spec.Width = 1280;
  spec.Height = 750;
  m_Framebuffer.reset(Perun::Graphics::Framebuffer::Create(spec));
  
  // Setup Blur Shader (Basic Box Blur or Gaussian approximation for now)
  // Rendering to a quad covering the screen.
  // Using a simple 9-tap blur or similar.
  std::string vs = R"(
      #version 450 core
      layout(location = 0) in vec2 a_Position;
      layout(location = 1) in vec2 a_TexCoord; // We probably don't have texcoords in DrawQuad?
      // Renderer::DrawQuad uses: in vec2 a_Position.
      // And we generate UVs? No, DrawQuad doesn't have UVs yet in Perun V1.
      // But we know a_Position is -0.5 to 0.5.
      // So UV = a_Position + 0.5.
      
      out vec2 v_TexCoord;
      
      void main() {
          v_TexCoord = a_Position + 0.5;
          gl_Position = vec4(a_Position * 2.0, 0.0, 1.0); // Scale 0.5 -> 1.0 for fullscreen
      }
  )";
  
  std::string fs = R"(
      #version 450 core
      out vec4 FragColor;
      in vec2 v_TexCoord;
      
      uniform sampler2D u_ScreenTexture;
      uniform vec4 u_Resolution; // Using vec4 to match SetFloat4
      
      void main() {
         // Simple Gaussian-ish Blur
         vec2 tex_offset = 1.0 / u_Resolution.xy; // gets size of single texel
         vec3 result = texture(u_ScreenTexture, v_TexCoord).rgb * 0.2270270270;
         result += texture(u_ScreenTexture, v_TexCoord + vec2(tex_offset.x * 1, 0.0)).rgb * 0.1945945946;
         result += texture(u_ScreenTexture, v_TexCoord - vec2(tex_offset.x * 1, 0.0)).rgb * 0.1945945946;
         result += texture(u_ScreenTexture, v_TexCoord + vec2(0.0, tex_offset.y * 1)).rgb * 0.1945945946;
         result += texture(u_ScreenTexture, v_TexCoord - vec2(0.0, tex_offset.y * 1)).rgb * 0.1945945946;
         // Note: True Gaussian is larger, this is a small blur.
         
         FragColor = vec4(result, 1.0);
      } 
  )";
  
  m_PostProcShader = std::make_unique<Perun::Graphics::Shader>(vs, fs);
  
  running = true;
}

Engine::~Engine()
{
  Perun::Renderer::Shutdown();
  // Window unique_ptr handles itself
}

void Engine::handleInput()
{
  window->PollEvents();
  if (window->ShouldClose()) {
      running = false;
  }
}

void Engine::render()
{
  // Bind Framebuffer BEFORE rendering scene
  m_Framebuffer->Bind();
  
  // Clear Framebuffer ?
  // We need to clear it, but Perun Renderer doesn't expose Clear.
  // We can just rely on covering it? No, particles move.
  // We need glClear.
  // Let's call glClear directly here since we linked OpenGL.
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT); // Depth? We don't use depth for 2D usually.

  // Begin Scene with Orthographic projection matching screen size
  float width = (float)window->GetWidth();
  float height = (float)window->GetHeight();
  
  Perun::Math::Matrix4 projection = Perun::Math::Matrix4::Orthographic(0.0f, width, height, 0.0f, -1.0f, 1.0f);
  
  Perun::Renderer::BeginScene(projection);
}

void Engine::update() 
{
    // Placeholder if we move swarm here
}

const bool Engine::isRunning()
{
  return running;
}

int Engine::getWidth() const { return window->GetWidth(); }
int Engine::getHeight() const { return window->GetHeight(); }

std::pair<int, int> Engine::getMousePosition() { return window->GetMousePosition(); }
bool Engine::isMouseButtonDown(int button) { return window->IsMouseButtonDown(button); }

void Engine::draw()
{
  // End scene (Draws particles to Framebuffer)
  Perun::Renderer::EndScene();
  m_Framebuffer->Unbind(); // Back to screen
  
  // Clear Screen
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw Framebuffer to Screen with Shader
  m_PostProcShader->Bind();
  // Bind texture
  glBindTexture(GL_TEXTURE_2D, m_Framebuffer->GetColorAttachmentRendererID());
  m_PostProcShader->SetInt("u_ScreenTexture", 0);
  m_PostProcShader->SetFloat4("u_Resolution", (float)window->GetWidth(), (float)window->GetHeight(), 0, 0); // Shader expects vec2 but SetFloat4 works if ignore others? Wait SetFloat4 implementation? 
  // Shader.h has SetFloat4 but not SetFloat2.
  // Let's use SetFloat4 and ignore z/w in shader or passing just 2 floats if possible?
  // OpenGL doesn't care if we pass more? No, SetFloat4 calls glUniform4f. If shader has vec2, it matches?
  // glUniform4f for vec2 uniform is invalid.
  // We need SetFloat2 in Shader class.
  // OR we can just hardcode tex_offset in shader or change shader to use vec4?
  // Let's change shader to: uniform vec4 u_Resolution; (use xy).
  
  // Actually, we can just use Perun::Renderer::DrawQuad but with OUR custom shader bound?
  // Perun::Renderer::DrawQuad binds its own shader!
  // We need Renderer::DrawQuadWithShader?
  // Or we just draw the quad manually here?
  // We have Quad VBO/VAO in RendererData, but correct way is to use Renderer.
  // Since Renderer doesn't support custom shaders yet easily for Fullscreen pass...
  // We can just add "DrawFullscreenQuad(texture)" to Renderer later.
  // For now: I will manually draw a quad here or use Renderer::DrawQuad and hope I can override shader?
  // No, DrawQuad Binds FlatColorShader.
  // Hack: DrawQuad binds shader.
  // So I can't easily use DrawQuad with my custom shader unless I modify Renderer.
  // Option: Modify Renderer to add `DrawFullscreenTexture(textureID)`.
  // Option: Draw manual GL here.
  // I'll draw manual GL here for speed, creating a dummy VAO.
  
  // Create a dummy VAO (empty) and draw arrays, generating vertices in vertex shader?
  // Or just create a static VAO here?
  static uint32_t quadVAO = 0;
  static uint32_t quadVBO;
  if (quadVAO == 0) {
      float quadVertices[] = { 
          // positions   // texCoords
          -1.0f,  1.0f,  0.0f, 1.0f,
          -1.0f, -1.0f,  0.0f, 0.0f,
           1.0f, -1.0f,  1.0f, 0.0f,

          -1.0f,  1.0f,  0.0f, 1.0f,
           1.0f, -1.0f,  1.0f, 0.0f,
           1.0f,  1.0f,  1.0f, 1.0f
      };
      glGenVertexArrays(1, &quadVAO);
      glGenBuffers(1, &quadVBO);
      glBindVertexArray(quadVAO);
      glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
      glEnableVertexAttribArray(0); // pos
      glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(1); // tex
      glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  }
  
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  
  window->SwapBuffers();
}
