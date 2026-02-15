#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>


using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : camera(45.0f, 0.1f, 100.0f) {
	
		Material& greenSphere = scene.materials.emplace_back();
		greenSphere.albedo = glm::vec3(1.0f, 0.6f, 0.0f);
		greenSphere.roughness = 0.3f;

		Material& blueSphere = scene.materials.emplace_back();
		blueSphere.albedo = glm::vec3(1.0f, 0.0f, 0.0f);
		blueSphere.roughness = 0.5f;


		{
			Sphere sphere;
			sphere.position = { 1.0f,0.0f,-5.3f };
			sphere.materialIndex = 0;
			sphere.radius = 2.0f;
			scene.spheres.push_back(sphere);
		}

		{
			Sphere sphere;
			sphere.position = { 1.0f,-51.0f,0.0f };
			sphere.materialIndex = 1;
			sphere.radius = 49.0f;
			scene.spheres.push_back(sphere);
		}
		{
			Light light;
			light.lightColor = glm::vec3{ 1.0f };
			light.lightDirection = glm::vec3(-2.0f, -1.0f, -2.0f);
			light.intensity = 1.3f;
			scene.lights.push_back(light);
		}

		{
			Light light;
			light.lightColor = glm::vec3{ 0.1f };
			light.lightDirection = glm::vec3(2.0f, -1.0f, -2.0f);
			light.intensity = 0.3f;
			scene.lights.push_back(light);
		}
	}
	
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render")) {
			Render();
		}
		ImGui::End();
		ImGui::Begin("Scene");
		for (int i = 0; i < scene.spheres.size(); i++)
		{
			ImGui::PushID(i);
			Sphere& sphere = scene.spheres[i];
			ImGui::DragFloat3("POSITION", glm::value_ptr(sphere.position), 0.1f);
			ImGui::DragFloat("RADIUS", &sphere.radius, 0.1f);
			ImGui::DragInt("MATERIAL INDEX", &sphere.materialIndex, 1.0f, 0, scene.materials.size());


			ImGui::Separator();
			ImGui::PopID();
		}

		for (int i = 0; i < scene.materials.size(); i++)
		{
			ImGui::PushID(i);
			Material&  material = scene.materials[i];
			ImGui::DragFloat("METALLIC", &material.metallic, 0.05f, 0.0f, 1.0f);
			ImGui::DragFloat("ROUGHNESS", &material.roughness, 0.05f, 0.0f, 1.0f);
			ImGui::ColorEdit3("SPHERE COLOR", glm::value_ptr(material.albedo));
			ImGui::Separator();
			ImGui::PopID();
		}

		for (int j = 0; j < scene.lights.size(); j++)
		{
			ImGui::PushID((int)j +1000);
			Light& light = scene.lights[j];
			ImGui::DragFloat3("Light Direction", glm::value_ptr(light.lightDirection), 0.1f);
			ImGui::ColorEdit3("Light Color", glm::value_ptr(light.lightColor));
			ImGui::DragFloat("Light Intensity", &light.intensity, 0.1f, 0.0f, 6.0f);
			ImGui::Separator();
			ImGui::PopID();
		}

		
		ImGui::End();


		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("ViewPort");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;
		

		auto image = renderer.GetFinalImage();
		if (image)
			ImGui::Image(image ->GetDescriptorSet(), {(float) image->GetWidth(), (float) image ->GetHeight()},ImVec2(0,1), ImVec2(1,0));

		ImGui::End();
		ImGui::PopStyleVar();

		Render();

	}

	virtual void OnUpdate(float timeStep) override{
		camera.OnUpdate(timeStep); 
	}

	void Render() {
		Timer timer;

		renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		renderer.Render(scene, camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer renderer;

	Camera camera;
	
	uint32_t m_ViewportWidth = 0, m_ViewportHeight =0;

	uint32_t* m_ImageData = nullptr;

	float m_LastRenderTime = 0.0f;

	Scene scene;

};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "My Path Tracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}