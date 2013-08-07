#include "GridModel.h"
#include "GLContext.h"
#include <time.h>
#include "KinectTool.h"
#include "TriangleMesh.h"


inline double diffclock( clock_t clock1, clock_t clock2 ) 
{
	double diffticks = clock1 - clock2;
    double diffms    = diffticks / ( CLOCKS_PER_SEC / 1000 );

    return diffms;
}

int main( int argc, UINT8** argv) 
{
	GLContext* cntx = new GLContext();
	Input* inp = new Input();
	cntx->SetInput( inp );

	unsigned int power = 8;
	unsigned int side = 1<<power;

	GridModel* model = new GridModel(power);//power of 2
	KinectTool* _tool = new KinectTool( (side*0.75f), (side*0.75f), side*.75f, -(side*.55f));

	inp->SetZoom(-(side*1.5f));

	//
	Color clr;
	clr.comp[0] = 255;
	clr.comp[1] = 255;
	clr.comp[2] = 255;
	clr.comp[3] = 255;
	model->UpdateCell(side-1, side-1, side-1, &clr);
	model->UpdateCell(side-1, side-1, 0, &clr);
	model->UpdateCell(side-1, 0, side-1, &clr);
	model->UpdateCell(0, side-1, side-1, &clr);
	model->UpdateCell(0, 0, side-1, &clr);
	model->UpdateCell(0, side-1, 0, &clr);
	model->UpdateCell(side-1, 0, 0, &clr);
	model->UpdateCell(0,0,0, &clr);
	//
	
	while (cntx->alive())
	{

		int acted = 0;
		clock_t start = clock();

		cntx->doMessage();		
		_tool->DoToolUpdate();
		acted = _tool->InteractModel( model, inp->GetObjectQ());
		model->UpdateGrid();
		cntx->renderScene(model, _tool->GetToolMesh(), inp->GetViewM(), inp->GetObjectM());

		clock_t end = clock();
		std::cout<<"Frame time = " << diffclock( end, start )<< " ms, " << "Interacted: "<< acted << std::endl;
	}
	
	delete model;
	delete cntx;
	delete inp;
	delete _tool;
	return 0;
}

