#pragma once

namespace bt
{
	enum EStatus
	{
		Invalid,
		Failure,
		Running,
		Success
	};
	
	class IBehavior
	{
	public:
		virtual void Initialize() {};
		virtual EStatus Update(float a_deltaTime) = 0;
		virtual void Terminate(EStatus) {};
	};
}
