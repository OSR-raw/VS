#include "Render.h"
#include "GraphicsLib.h"
#include "GridModel.h"
#include "VAO.h"
#include "KinectTool.h"
#include "TriangleMesh.h"
#include <time.h>


Render::Render(): shader(NULL)
{
	windowWidth = 512;
	windowHeight = 512;
	Point center;
	center.coord[0] = 0.0;
	center.coord[1] = 0.0;
	center.coord[2] = 0.0;
	Color clr;
	clr.comp[0] = 255;
	clr.comp[1] = 255;
	clr.comp[2] = 255;
	clr.comp[3] = 255;
}


Render::~Render()
{
	if (shader)
		delete shader;
}

void Render::Init()
{
	SetupScene();
}

void Render::SetupScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	shader = new Shader();
	shader->loadFragmentShader("Shaders/shader.frag");
	shader->loadGeometryShader("Shaders/shader.geom");
	shader->loadVertexShader("Shaders/shader.vert");
	shader->link();
			
	pvmMatrixLocation = glGetUniformLocation(shader->id(), "pvm");
	
	projectionMatrix = glm::perspective(120.0f, (float)windowWidth / (float)windowHeight, 0.01f, 1000.f);
}

inline double diffclock( clock_t clock1, clock_t clock2 ) 
{
	double diffticks = clock1 - clock2;
    double diffms    = diffticks / ( CLOCKS_PER_SEC / 1000 );

    return diffms;
}

void Render::Draw( GridModel* model, KinectTool* tool, glm::mat4& view, glm::mat4& obj )
{
	clock_t start = clock();

	glm::mat4 pvm = projectionMatrix*view*obj;

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	int i = 0;
	shader->bind();
	glUniformMatrix4fv(pvmMatrixLocation, 1, GL_FALSE, &(pvm[0][0]));
	
	std::map< unsigned int, VAO* >* cells = model->GetRenderableCells();
	std::map< unsigned int, VAO* >::iterator iter;
	VAO* vao_ptr = NULL;
	for (iter = cells->begin(); iter != cells->end(); iter++)
	{
		vao_ptr = iter->second;
		glBindVertexArray(vao_ptr->id());
		 
		glDrawElements(
			 GL_POINTS,      // mode
			 vao_ptr->size(),    // count
			 GL_UNSIGNED_INT,   // type
			 (void*)0           // element array buffer offset
		 );
		i++;
	}
	
	glBindVertexArray(0);
	shader->unbind();
	
	pvm = projectionMatrix*view;

	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);	
	glDepthMask(GL_FALSE);
	vao_ptr = tool->GetToolMesh()->GetVAO();

	tool->GetToolShader()->bind();	
	glUniformMatrix4fv(tool->GetPVMLocation(), 1, GL_FALSE, &(pvm[0][0]));
	
	glBindVertexArray(vao_ptr->id());
	glDrawElements(
			GL_TRIANGLES,      // mode
			 vao_ptr->size(),    // count
			 GL_UNSIGNED_INT,   // type
			 (void*)0           // element array buffer offset
		 );	
	glBindVertexArray(0);
	tool->GetToolShader()->unbind();
	
	///
	clock_t end = clock();
	//std::cout<<"For ticks = "<<i<<", tick time = " << diffclock( end, start )<< " ms" << std::endl;
}

void Render::Resize(int w, int h)
{
	glViewport(0, 0, w,  h);
	windowWidth = w;
	windowHeight = h;
	projectionMatrix = glm::perspective(30.0f, (float)windowWidth / (float)windowHeight, 0.01f, 2048.f);
}