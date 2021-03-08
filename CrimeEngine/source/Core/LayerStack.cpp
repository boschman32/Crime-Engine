#include "cepch.h"
#include "Core/LayerStack.h"

namespace ce
{
	LayerStack::LayerStack()
		: m_layerInsertIndex(0)
	{
		
	}

	LayerStack::~LayerStack()
	{
		for(auto &layer : m_layers)
		{
			layer->OnDetach();
		}
	}

	Layer* LayerStack::PushLayer(std::unique_ptr<Layer> a_layer)
	{
		m_layers.emplace(m_layers.begin() + m_layerInsertIndex, std::move(a_layer));
		m_layers[m_layerInsertIndex]->OnAttach();
		m_layerInsertIndex++;

		return m_layers[m_layerInsertIndex - 1].get();
	}

	Layer* LayerStack::PushOverlay(std::unique_ptr<Layer> a_overlay)
	{
		m_layers.emplace_back(std::move(a_overlay));
		m_layers.back()->OnAttach();

		return m_layers.back().get();
	}

	void LayerStack::PopLayer(Layer& a_layer)
	{
		const auto it = std::find_if(m_layers.begin(), m_layers.begin() + m_layerInsertIndex, [&a_layer](const auto& a_l)
		{
				return &a_layer == a_l.get();
		});

		if(it != m_layers.end())
		{
			a_layer.OnDetach();
			m_layers.erase(it);
			m_layerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer& a_overlay)
	{
		const auto it = std::find_if(m_layers.begin() + m_layerInsertIndex, m_layers.end(), [&a_overlay](const auto& a_l)
			{
				return &a_overlay == a_l.get();
			});

		if (it != m_layers.end())
		{
			a_overlay.OnDetach();
			m_layers.erase(it);
		}
	}
}
