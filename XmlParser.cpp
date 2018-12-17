#include "XmlParser.h"
#include <iostream>

XmlParser::XmlParser(const std::string& buf) : buf_(buf)
{
	pos.push_back(Position{0,buf_.size()});
}

XmlParser& XmlParser::parse(const std::string& nodeName)
{
	std::vector<Position> p;
	for(unsigned i = 0; i < pos.size(); ++i)
	{
		std::size_t pos1 = pos[i].start;
		int count = 0;
		while(true)
		{
			pos1 = buf_.find("<"+nodeName+">", pos1);
			if(pos1 == std::string::npos || pos1 > pos[i].end)
				break;
			p.push_back(Position{pos1+nodeName.size()+2, 0});
			++pos1;
			++count;
		}

		if (!count)
			throw std::runtime_error("XmlParser::parse error: node name not found");
		
		pos1 = pos[i].start;
		int count2 = 0;
		for(unsigned i2 = p.size()-count; i2 < p.size(); ++i2)
		{
			pos1 = buf_.find("</"+nodeName+">", pos1);
			if(pos1 > pos[i].end)
				break;
			p[i2].end = pos1;
			++pos1;
			++count2;
		}

		if (count != count2)
			throw std::runtime_error("XmlParser::parse error: xml element is missing");
	}
	pos = p;
	return *this;
}

std::vector<std::string> XmlParser::getValues()
{
	std::vector<std::string> v;
	for(auto& a: pos)
	{
		v.push_back(buf_.substr(a.start, a.end-a.start));
	}
	resetPosition();
	return v;
}

XmlParser& XmlParser::insertNode(const std::string& nodeName)
{
	std::string strToInsert = "\n<"+nodeName+"></"+nodeName+">";
	std::size_t newSize = strToInsert.size();
	for(unsigned i = 0; i < pos.size(); ++i)
	{
		for(unsigned i2 = i; i2 < pos.size(); ++i2)
		{
			pos[i2].start += newSize;
			pos[i2].end += newSize;
		}
		buf_.insert(pos[i].start-newSize, strToInsert);
		pos[i].start-=nodeName.size()+3;
	}
	return *this;
}

void XmlParser::setValues(const std::vector<std::string>& values)
{
	if (values.size() != pos.size())
		throw std::runtime_error("XmlParser::setValues error: vector size != node count");
	
	for(unsigned i = 0; i < pos.size(); ++i)
	{
		std::string strToInsert = values[i];
		std::size_t newSize = strToInsert.size();
		for(unsigned i2 = i; i2 < pos.size(); ++i2)
		{
			pos[i2].start += newSize;
			pos[i2].end += newSize;
		}
		buf_.insert(pos[i].start-newSize, strToInsert);
	}
	resetPosition();
}

void XmlParser::resetPosition()
{
	pos.erase(pos.begin()+1, pos.end());
	pos[0].start = 0;
	pos[0].end = buf_.size();
}

const std::string& XmlParser::getBuf() const
{
	return buf_;
}