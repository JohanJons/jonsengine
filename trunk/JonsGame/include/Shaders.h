#pragma once

#include <string>

namespace JonsGame
{
	const std::string gVertexShader =	"#version 330								\n	\
																					\n	\
										in vec3 position;							\n	\
										void main()									\n	\
										{											\n	\
											gl_Position = vec4(position, 1.0f);		\n	\
										}											\n";


	const std::string gFragmentShader =	"#version 330									\n \
																						\n \
										out vec4 outColor;								\n \
										void main()										\n \
										{												\n \
											outColor = vec4(1.0f, 0.5f, 0.5f, 1.0f);	\n \
										}												\n";
}