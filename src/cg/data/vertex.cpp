#include "cg/data/vertex.h"



namespace cg {
namespace data {

// ----- funcs -----

std::ostream& operator<<(std::ostream& out, const Vertex_attribs& attribs)
{
	out << "Vertex_attribs(";

	if (attribs == Vertex_attribs::none) {
		out << "none";
	}
	else {
		if (has_position(attribs)) out << "position";
		if (has_normal(attribs)) out << "normal ";
		if (has_tex_coord(attribs)) out << "tex_coord ";
		if (has_tangent_space(attribs)) out << "tangent_h";
	}

	out << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex_attribs& attribs)
{
	out << "Vertex_attribs(";

	if (attribs == Vertex_attribs::none) {
		out << "none";
	}
	else {
		if (has_position(attribs)) out << "position";
		if (has_normal(attribs)) out << "normal ";
		if (has_tex_coord(attribs)) out << "tex_coord ";
		if (has_tangent_space(attribs)) out << "tangent_h";
	}

	out << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const Vertex_old& v)
{
	out << "Vertex_old(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan_h: " << v.tangent_h << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex_old& v)
{
	out << "Vertex_old(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan_h: " << v.tangent_h << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const Vertex& v)
{
	out << "Vertex(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex& v)
{
	out << "Vertex_ts(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord << ")";
	return out;
}

std::ostream& operator<<(std::ostream& out, const Vertex_ts& v)
{
	out << "Vertex_ts(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan: " << v.tangent
		<< "; bitan: " << v.bitangent << ")";
	return out;
}

std::wostream& operator<<(std::wostream& out, const Vertex_ts& v)
{
	out << "Vertex_ts(p: " << v.position
		<< "; n: " << v.normal
		<< "; tc: " << v.tex_coord
		<< "; tan: " << v.tangent
		<< "; bitan: " << v.bitangent << ")";
	return out;
}

size_t get_hash(const Vertex& v) noexcept
{
	float l = len(v.position);
	return std::hash<float>{}(l);
}

} // namespace data
} // namespace cg