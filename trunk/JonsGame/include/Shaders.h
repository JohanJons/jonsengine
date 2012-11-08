#pragma once

#include <string>

namespace JonsGame
{
	const std::string gVertexShader =	"#version 330								\n	\
																					\n	\
										in vec3 in_position;						\n	\
                                        in vec3 in_color;                           \n	\
                                        out vec3 ex_color;                          \n	\
										void main()									\n	\
										{											\n	\
											gl_Position = vec4(in_position, 1.0f);	\n	\
                                            ex_color = in_color;                    \n	\
										}											\n";


	const std::string gFragmentShader =	"#version 330									\n \
										in vec3 ex_color;								\n \
										out vec4 outColor;								\n \
										void main()										\n \
										{												\n \
											outColor = vec4(ex_color, 1.0f);	        \n \
										}												\n";
}