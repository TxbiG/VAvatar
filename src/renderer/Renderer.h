

#include "../variants.h"
#include "../resources.h"

struct Renderer;

Renderer* Create_Renderer();
void Present_Renderer(Renderer* renderer);


// Presenting
void Renderer_Present_Rect(Rect rect, Color color);
void Renderer_Present_TextureRect(Rect rect, Texture texture);

void Renderer_Present_Mesh(Mesh mesh);
void Renderer_Present_Modal(Modal modal);