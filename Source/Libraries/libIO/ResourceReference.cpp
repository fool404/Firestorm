///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ResourceReference.cpp
//
//  A reference to a resource. Contains an instance of File.
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Copyright (c) Project Firestorm 2018
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceReference.h"

OPEN_NAMESPACE(Firestorm);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FIRE_MIRROR_DEFINE(Firestorm::ResourceReference)
// {
// 	Class
// 		.property("resourcePath", &ResourceReference::_resourcePath);
// }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceReference::ResourceReference(const string& path)
: _resourcePath(path)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ResourceReference::~ResourceReference()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const string& ResourceReference::GetPath() const
{
	return _resourcePath;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

string ResourceReference::GetPathTo() const
{
	auto split = SplitString(_resourcePath, '/');
	split.pop_back();
	string out;
	for(auto str : split)
		out += str + "/";
	return out;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ResourceReference::SetResourcePath(const string& path)
{
	_resourcePath = path;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CLOSE_NAMESPACE(Firestorm);
