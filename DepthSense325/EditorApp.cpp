#include "EditorApp.h"

#include "BoneManipulation.h"
#include "Context.h"
#include "HandVisualization.h"
#include "Models.h"
#include "ModelRotation.h"
#include "ModelPainter.h"
#include "Import.h"

namespace mobamas {

Polycode::SceneMesh* LoadMesh(Models model) {
	Polycode::SceneMesh* mesh;
	switch (model) {
	case Models::MIKU:
		mesh = new SceneMesh("Resources/tdamiku.mesh");
		mesh->loadTexture("Resources/tdamiku.png");
		mesh->loadSkeleton("Resources/tdamiku.skeleton");
		break;
	case Models::ROBOT:
		mesh = new SceneMesh("Resources/dummy.mesh");
		mesh->loadTexture("Resources/dummy.png");
		mesh->loadSkeleton("Resources/dummy.skeleton");
		mesh->Scale(3, 3, 3);
		break;
	default:
		std::cout << "Unknown model" << std::endl;
		assert(false);
		break;
	}
	return mesh;
}

Polycode::Entity* LoadMesh2(Models model) {
	Polycode::Entity* mesh;
	switch (model) {
	case Models::MIKU:
		mesh = importCollada("Resources/test2.DAE");
		break;
	default:
		std::cout << "Unknown model" << std::endl;
		assert(false);
		break;
	}
	return mesh;
}



EditorApp::EditorApp(PolycodeView *view, std::shared_ptr<Context> context) {
	core_ = new POLYCODE_CORE(view, kWinWidth, kWinHeight, false, true, 0, 0, 90);

	core_->enableMouse(false);

	auto rm = Polycode::CoreServices::getInstance()->getResourceManager();
	rm->addArchive("Resources/default.pak");
	rm->addDirResource("default", false);

	auto scene = new Polycode::Scene();

	mesh_ = LoadMesh(context->model);
	scene->addEntity(mesh_);
	mesh2_ = LoadMesh2(context->model);
	scene->addEntity(mesh2_);
	scene->useClearColor = false;

	auto light = new SceneLight(SceneLight::POINT_LIGHT, scene, 100);
	light->setPosition(7, 7, 7);
	scene->addLight(light);
	scene->enableLighting(true);

	auto skeleton = mesh_->getSkeleton();
	Polycode::Bone* root = nullptr;
	for (unsigned int bidx = 0; bidx < skeleton->getNumBones(); bidx++)
	{
		auto b = skeleton->getBone(bidx);
		if (b->getParentBone() == nullptr) {
			root = b;
			break;
		}
	}
	root->setPositionY(root->getPosition().y - 1); // adjust center to rotate

	auto cam = 	scene->getActiveCamera();
	cam->setPosition(0, 0, 20);
	cam->lookAt(Polycode::Vector3(0, 0, 0));

	hand_visualization_.reset(new HandVisualization(scene, context->rs_client));
	bone_manipulation_.reset(new BoneManipulation(context, scene, mesh_, context->model));
	painter_.reset(new ModelPainter(context, scene, mesh2_));
	rotation_.reset(new ModelRotation(context, mesh2_));

	context->pinch_listeners = bone_manipulation_;
}

EditorApp::~EditorApp() {
	if (mesh_ != nullptr)
		delete mesh_;
	if (core_ != nullptr)
		delete core_;
}

bool EditorApp::Update() {
	bone_manipulation_->Update();
	hand_visualization_->Update();
	return core_->updateAndRender();
}

}