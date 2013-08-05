class GridModel;
#include "Shader.h"
#include "main.h"

class TriangleMesh;

class Render
{
public:
	Render();
	~Render();
	void Init();
	void SetupScene();
	void Resize(int w, int h);
	void Draw( GridModel* model, TriangleMesh* _tool_mesh, glm::mat4& view, glm::mat4& obj );
private:
	glm::mat4 projectionMatrix;

	int windowWidth;
	int windowHeight;

	Shader *shader;
	int pvmMatrixLocation;
	int colorLocation;
	int objLocation;

	Shader* mesh_shader;
	int pvmLocMesh;	
};

