#pragma once
#if CE_DEBUG
#define CE_ENABLE_ASSERT
#endif

#ifdef CE_ENABLE_ASSERT
#define VA_ARGS(...) , ##__VA_ARGS__

#define CE_ASSERT(assertion)\
{\
	if(!(assertion))\
	{\
		std::string assertmsg = std::string("Assertion `"#assertion"` failed in ") + std::string(__FILE__) + " on line: " + std::to_string(__LINE__) + ": ";\
		CE_APP_CRITICAL(assertmsg);\
		std::terminate();\
	}\
}

#define CE_ASSERT_M(assertion, msg, ...)\
{\
	if(!(assertion))\
	{\
		std::string assertmsg = std::string("Assertion `"#assertion"` failed in ") + std::string(__FILE__) + " on line: " + std::to_string(__LINE__) + ": ";\
		CE_APP_CRITICAL(assertmsg + std::string(msg) VA_ARGS(__VA_ARGS__));\
		std::terminate();\
	}\
}

#define CE_CORE_ASSERT(assertion)\
{\
	if(!(assertion))\
	{\
		std::string assertmsg = std::string("Assertion `"#assertion"` failed in ") + std::string(__FILE__) + " on line: " + std::to_string(__LINE__) + ": ";\
		CE_CORE_CRITICAL(assertmsg);\
		std::terminate();\
	}\
}
#define CE_CORE_ASSERT_M(assertion, msg, ...)\
{\
	if(!(assertion))\
	{\
		std::string assertmsg = std::string("Assertion `"#assertion"` failed in ") + std::string(__FILE__) + " on line: " + std::to_string(__LINE__) + ": ";\
		CE_CORE_CRITICAL(assertmsg + std::string(msg) VA_ARGS(__VA_ARGS__));\
		std::terminate();\
	}\
}
#else
#define CE_ASSERT(a)
#define CE_ASSERT_M(a, msg, ...)
#define CE_CORE_ASSERT(a)
#define CE_CORE_ASSERT_M(a, msg, ...)
#endif

template<typename T, typename IdType = int>
class TypeSafeId
{
public:
	explicit TypeSafeId(IdType a_id = IdType())
		: m_id(a_id) {}
	virtual ~TypeSafeId() = default;

	IdType Value() const noexcept { return m_id; }

	bool operator<(TypeSafeId<T, IdType> a_rhs) const noexcept { return m_id < a_rhs.m_id; }
	bool operator==(TypeSafeId<T, IdType> a_rhs) const noexcept { return m_id == a_rhs.m_id; }
	bool operator!=(TypeSafeId<T, IdType> a_rhs) const noexcept { return m_id != a_rhs.m_id; }

	template<typename OStream>
	friend OStream& operator<<(OStream &a_os, const TypeSafeId<T, IdType>& a_identifier)
	{
		return a_os << a_identifier.Value();
	}
private:
	IdType m_id;
};

//Core Id's
namespace ce
{
	class Resource;
	using ResourceId = TypeSafeId<Resource, uint64_t>;

	class GameObject;
	using GameObjectId = TypeSafeId<GameObject, uint32_t>;
}