#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>

#define __DYML_IMPLEMENTATION__
#include "../dyml.h"
using namespace dyml;

#include "dyml-helper.h"

size_t loadFromFile(const char* path, string& memFile)
{
	size_t sof = 0;

	auto fp = fopen(path, "rb");
	if (fp)
	{
		fseek(fp, 0, SEEK_END);
		const auto sof = (size_t)ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if (sof > 0)
		{
			memFile.resize(sof);
			fread(&memFile[0], 1, sof, fp);
		}

		fclose(fp);
	}

	return sof;
}



inline void print_spaces(int cnt)
{
	for (int i = 0; i < cnt; ++i) putchar(' ');
}

void print_yaml_tree(Directyaml::Node node, int level)
{
	int noc = node.children();

	for (int i = 0; i < noc; ++i)
	{
		// leading spaces
		printf("%s", string((level + 1) * 2, ' ').c_str());

		// key-value
		auto ch = node.child(i);

		const auto k = ch.key();
		const auto v = ch.val();

		if (k) printf("%s: ", k); // as key
		//else
			//printf("- "); // (key is null, print as array)

		if (v) printf("%s\n", v);
		else
			printf("\n");

		print_yaml_tree(ch, level + 1);
	}
}

void print_yaml_rows(Directyaml& my, int width)
{
	char sn[0x20];

	const auto& rows = my.rows();

	const int cnt = (int)rows.size();

	for (int i = 0; i < cnt; ++i)
	{
		const auto& row = rows[i];

		sprintf(sn, "#%d", i);
		printf("%5s lv=%2d %*s | %s\n"
			, sn
			, row.level
			, width
			, row.key ? row.key : " "
			, row.val ? row.val : " ")
			;
	}
}

void print_yaml(const char* yaml_path, int key_width)
{
	string memFile;

	loadFromFile(yaml_path, memFile);

	Directyaml dym;
	dym.parse(&memFile[0]);

	printf("%s: ------->\n", yaml_path);

	printf("By rows:\n");
	print_yaml_rows(dym, key_width);

	printf("\nBy hierarchy:\n");
	print_yaml_tree(dym.root(), -1);

	printf("\n%s <-------\n\n", yaml_path);
}

int main(int argc, char* argv[])
{
	SetConsoleOutputCP(CP_UTF8);

	print_yaml("./ymls/var.yaml", 8);

	print_yaml("./ymls/rifle.yaml", 20);

	print_yaml("./ymls/comments.yaml", 10);

	print_yaml("./ymls/yaml.yaml", 44);

	// load values
	{
		string memFile;
		loadFromFile("./ymls/rifle.yaml", memFile);

		Directyaml dym(&memFile[0]);

		// simple values
		{
			auto node = dym.root()["GeneralSettings"];

			float fov;
			MyNode(node["FieldOfView"]).value(fov);

			fov = MyNode(node["FieldOfView"]).read<float>();

			vec3 pivot;
			MyNode(node["PivotPosition"]).value(pivot);

			pivot = MyNode(node["PivotPosition"]).read<vec3>();
		}

		// simple array
		{
			auto node = dym.root()["GeneralSettings"]["Colors"];

			const char* colors[0x20];

			auto noc = node.children();
			for (int i = 0; i < noc; ++i)
			{
				colors[i] = node[i].val();
			}
		}

		// struct array
		{
			struct Force
			{
				float delay;
				vec3 force;
				float distribution;
			};

			Force forces[0x10];

			auto node = dym.root()["CamForces"]["EmptyReloadLoop"];

			int noc = node.children();
			for (int i = 0; i < (noc / 3); ++i)
			{
				auto& f = forces[i];
				MyNode(node[i * 3 + 0]).value(f.delay);
				MyNode(node[i * 3 + 1]).value(f.force);
				MyNode(node[i * 3 + 2]).value(f.distribution);
			}
		}
	}

	{
		Directyaml dy;

		{
			string memFile;
			loadFromFile("./ymls/rifle.yaml", memFile);

			// managed test
			dy.parse(&memFile[0], true);
		}

		print_yaml_rows(dy, 20);
	}

	system("pause");

	return 0;
}
