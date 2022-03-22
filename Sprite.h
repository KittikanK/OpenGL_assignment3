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

class Sprite
{
public:
    Sprite(Mesh* shape);
    ~Sprite();

    void Draw(GLuint uniformModel);

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    Mesh* mesh;
private:
};
