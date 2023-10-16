#include "MotionAlongPathViewer.h"

#include "../../../../Engine/src/Core/Scene/GameObject.h"
#include "../../../../Engine/src/Core/Scene/Components/LineRendererComponent.h"

namespace SampleApplication
{
	MotionAlongPathViewer::MotionAlongPathViewer()
	{

	}

	void MotionAlongPathViewer::Awake()
	{
		std::vector<glm::vec3> p;
		p.push_back(glm::vec3(1, 1, 0));
		p.push_back(glm::vec3(2, 1, 0));
		p.push_back(glm::vec3(3, 2, 0));
		p.push_back(glm::vec3(4, 2, 0));
		p.push_back(glm::vec3(5, 3, 0));
		p.push_back(glm::vec3(6, 3, 0));
		spaceCurve.SetControlPoints(p);


		lineRenderer = gameObject->GetComponent<IHCEngine::Component::LineRendererComponent>();
		lineRenderer->SetPoints(spaceCurve.GetPoints());
	}

	void MotionAlongPathViewer::Start()
	{

	}

	void MotionAlongPathViewer::Update()
	{

	}

	void MotionAlongPathViewer::FixedUpdate(){}

	void MotionAlongPathViewer::OnEnable(){}

	void MotionAlongPathViewer::OnDisable(){}
}
