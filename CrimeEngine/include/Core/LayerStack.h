#pragma once
#include "Layer.h"
#include <vector>
#include <memory>

namespace ce
{
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();
		
		Layer* PushLayer(std::unique_ptr<Layer> a_layer);
		Layer* PushOverlay(std::unique_ptr<Layer> a_overlay);
		void PopLayer(Layer& a_layer);
		void PopOverlay(Layer& a_overlay);

		std::vector<std::unique_ptr<Layer>>::iterator begin() //NOLINT
		{ return m_layers.begin(); } 
		std::vector<std::unique_ptr<Layer>>::iterator end() //NOLINT
		{ return m_layers.end(); }
	private:
		std::vector<std::unique_ptr<Layer>> m_layers;
		std::size_t m_layerInsertIndex { 0 };
	};
}