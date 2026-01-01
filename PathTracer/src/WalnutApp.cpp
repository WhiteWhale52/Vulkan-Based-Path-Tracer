#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"

#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Renderer.h"
#include "Camera.h"


using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	ExampleLayer() : camera(45.0f, 0.1f, 100.0f) {}
	
	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render: %.3fms", m_LastRenderTime);
		if (ImGui::Button("Render")) {
			Render();
		}
		static float color[3] = { (float)renderer.sphereColor.r, (float)renderer.sphereColor.g,  (float)renderer.sphereColor.b };

		ImGui::ColorPicker3("Sphere Color", color, 1);
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
		renderer.Render(camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private:
	Renderer renderer;

	Camera camera;
	
	uint32_t m_ViewportWidth = 0, m_ViewportHeight =0;

	uint32_t* m_ImageData = nullptr;

	float m_LastRenderTime = 0.0f;

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