#ifndef __DIRECT_YAML_HELPER_H__
#define __DIRECT_YAML_HELPER_H__

namespace dyml
{
	struct vec3 { float x, y, z; };

	class MyNode
	{
	public:
		MyNode(Directyaml::Node node) : _node(node) {}
		~MyNode() {}

	public:
		template <typename T>
		T read() const {}

		template <>
		float read() const { return strtof(_node.val(), nullptr); }

		template <>
		vec3 read() const
		{
			vec3 v;
			sscanf(_node.val(), "[%f,%f,%f]", &v.x, &v.y, &v.z);
			return v;
		}


		template <typename T>
		void value(T& v) const {}

		// impl.
		template <> void value(const char*& v) const { v = _node.val(); }

		template <> void value(int& v) const { v = strtol(_node.val(), nullptr, 10); }

		template <>	void value(float& v) const { v = strtof(_node.val(), nullptr); }

		template <>	void value(vec3& v) const { sscanf(_node.val(), "[%f,%f,%f]", &v.x, &v.y, &v.z); }

	protected:
		Directyaml::Node _node;
	};
}

#endif //__DIRECT_YAML_HELPER_H__