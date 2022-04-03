#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <cmath>

#include "Libs/Shader.h"
#include "Libs/Window.h"
#include "Libs/Mesh.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Block
{
public:
    Block(Mesh* shape, unsigned int text);
    ~Block();

    void Draw(GLuint uniformModel);

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
private:
    Mesh* mesh;
    unsigned int texture;
};
