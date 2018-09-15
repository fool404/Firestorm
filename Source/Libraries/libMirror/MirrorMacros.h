#if !defined LIBMIRROR_MIRRORMACROS_H_
#define LIBMIRROR_MIRRORMACROS_H_
#pragma once

#if defined FIRE_DEBUG || FIRE_RELEASE
/**
	Macro that provides more functions to a reflected object.

	static MyType   - Retrieves a consistent type.
	static Is       - Checks the type against all common forms.
	virtual GetType - Retrieve the type at the instance leve.
**/
	#define FIRE_MIRROR_DECLARE(ObjectType, ...)                                                                   \
		private:                                                                                                   \
			static bool s_isRegistered;                                                                            \
			template <class T> friend struct Library;                                                              \
			using register_type = rttr::registration::class_<ObjectType>;                                          \
			static void DoRegisterReflection(register_type& Registrar);                                            \
		public:                                                                                                    \
			static void RegisterReflection();                                                                      \
			static rttr::type MyType() {																		   \
				FIRE_ASSERT(ObjectType::s_isRegistered && "object of type "#ObjectType" is not yet registered.");  \
				return rttr::type::get<ObjectType>();															   \
			}																									   \
			virtual RTTR_INLINE rttr::type GetType() const {													   \
				FIRE_ASSERT(ObjectType::s_isRegistered && "object of type "#ObjectType" is not yet registered.");  \
				return ObjectType::MyType();																	   \
			}																									   \
			virtual RTTR_INLINE rttr::type get_type() const {													   \
				FIRE_ASSERT(ObjectType::s_isRegistered && "object of type "#ObjectType" is not yet registered.");  \
				return rttr::detail::get_type_from_instance(this);												   \
			}																									   \
			virtual RTTR_INLINE void* get_ptr() {																   \
				FIRE_ASSERT(ObjectType::s_isRegistered && "object of type "#ObjectType" is not yet registered.");  \
				return reinterpret_cast<void*>(this);															   \
			}																									   \
			virtual RTTR_INLINE rttr::detail::derived_info get_derived_info() {                                    \
				FIRE_ASSERT(ObjectType::s_isRegistered && "object of type "#ObjectType" is not yet registered.");  \
				return {                                                                                           \
					reinterpret_cast<void*>(this),                                                                 \
					rttr::detail::get_type_from_instance(this)                                                     \
				};                                                                                                 \
			}                                                                                                      \
			using base_class_list = rttr::detail::type_list<__VA_ARGS__>;                                          \
		private:                                                                                                   \
			template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer> 				   \
			friend struct rttr::detail::constructor_invoker
#elif defined FIRE_FINAL

	#define FIRE_MIRROR_DECLARE(ObjectType, ...)
		private:                                                                                                   \
			template <class T> friend struct Library;                                                              \
			using register_type = rttr::registration::class_<ObjectType>;                                          \
			static void DoRegisterReflection(register_type& Registrar);                                            \
		public:                                                                                                    \
			static void RegisterReflection();                                                                      \
			static rttr::type MyType() { return rttr::type::get<ObjectType>(); }								   \
			virtual RTTR_INLINE rttr::type GetType() const { return ObjectType::MyType(); }                        \
			virtual RTTR_INLINE rttr::type get_type() const { return rttr::detail::get_type_from_instance(this); } \																									   \
			virtual RTTR_INLINE void* get_ptr() { return reinterpret_cast<void*>(this); }                          \
			virtual RTTR_INLINE rttr::detail::derived_info get_derived_info() {                                    \
				return {                                                                                           \
					reinterpret_cast<void*>(this),                                                                 \
					rttr::detail::get_type_from_instance(this)                                                     \
				};                                                                                                 \
			}                                                                                                      \
			using base_class_list = rttr::detail::type_list<__VA_ARGS__>;                                          \
		private:                                                                                                   \
			template<typename Ctor_Type, typename Policy, typename Accessor, typename Arg_Indexer> 				   \
			friend struct rttr::detail::constructor_invoker

#endif // FIRE_MIRROR_DECLARE macro

#if defined FIRE_DEBUG || FIRE_RELEASE
	#define FIRE_MIRROR_DEFINE_NAMED(ObjectType, ObjectName)			                       \
		bool ObjectType::s_isRegistered = false;                                               \
		void ObjectType::RegisterReflection()                                                  \
		{                                                                                      \
			FIRE_ASSERT(ObjectType::s_isRegistered == false &&                                 \
				"object of type '"#ObjectType"' is already registered for reflection");        \
			ObjectType::s_isRegistered = true;                                                 \
			std::cout<<"    :: Registering Class Type: "<<ObjectName<<std::endl<<std::flush;   \
			ObjectType::register_type registration(ObjectName);                                \
			ObjectType::DoRegisterReflection(registration);                                    \
		}                                                                                      \
		void ObjectType::DoRegisterReflection(register_type& Class)

#elif defined FIRE_FINAL
	#define FIRE_MIRROR_DEFINE_NAMED(ObjectType, ObjectName)                               \
		void ObjectType::RegisterReflection()                                              \
		{                                                                                  \
			ObjectType::s_isRegistered = true;                                             \
			rttr::registration::class_<ObjectType> registration(ObjectName);               \
			ObjectType::DoRegisterReflection(registration);                                \
		}                                                                                  \
		void ObjectType::DoRegisterReflection(register_type& Class)
#endif // FIRE_MIRROR_DEFINE_NAMED macro

/**
	Define a class for the reflection system.
 **/
#define FIRE_MIRROR_DEFINE(ObjectType) FIRE_MIRROR_DEFINE_NAMED(ObjectType, #ObjectType)

#endif
