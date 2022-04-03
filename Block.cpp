#include "Block.h"

Block::Block(Mesh* shape, unsigned int text)
{
    mesh = shape;
    texture = text;
    
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

Block::~Block()
{
    mesh = NULL;
}

void Block::Draw(GLuint uniformModel)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    mesh->RenderMesh();
    
}