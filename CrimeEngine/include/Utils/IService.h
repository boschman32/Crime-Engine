#pragma once

namespace ce
{
	class IService
	{
	public:
		IService() = default;
		virtual ~IService() = default;

		virtual void OnCreate() = 0;
		virtual void OnDestroy() = 0;
	};
}