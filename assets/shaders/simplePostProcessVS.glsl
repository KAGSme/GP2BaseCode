#version 150

//The input vertex
in vec2 vertexPosition;
out vec2 textureCoords;

//The main function
void main()
{
  textureCoords = (vertexPosition + 1.0)/2.0;
  gl_Position = vec4(VertexPosition, 0.0, 1.0);
}
