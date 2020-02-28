#include "../Resources/ResourceManager.h"

#include "stb_image.h"

namespace Rscs 
{
	void log(const std::string &msg, Debug::InfoType type)
	{
		Debug::Log(msg, type, { "Resources", "IO" });
	}

	WriteTime getLastWriteTime(const Path &path)
	{
		return std::filesystem::last_write_time(path);
	}


	Manager::Manager()
	{ Locator::Add(Locator::CmdNode("Update", this, &Manager::update)); }

	Manager &Manager::Get()
	{
		static Manager instance;
		return instance;
	}

	FileRef Manager::getFile(const Path &path)
	{
		FileRef filePtr;
		if (auto &fileIter = loadedFiles.find(path); fileIter != loadedFiles.end())
		{
			filePtr = fileIter->second;
		}
		else
		{
			FileRef file = loadFile(path);
			filePtr = loadedFiles.emplace(path, file).first->second;
		}
		return filePtr;
	}

	FileRef Manager::loadFile(const Path &path)
	{
		FileType fileType = getFileType(path);
		FileRef result;

		if(fileType == Text)
		{
			std::ifstream stream(path, std::ios::in);

			if (stream.is_open())
			{
				std::stringstream sstr;
				sstr << stream.rdbuf();

				TextFileInfo file;
				file.Path_ = path;
				file.Contents = sstr.str();
				file.LastWriteTime = getLastWriteTime(path);
				result = std::make_shared<FileInfo>(new FileInfo(dynamic_cast<FileInfo &>(file)));

				stream.close();
			}
			else
			{
				log("Could not open file at " + path + ".", Debug::Error);
			}
		}
		else if(fileType == Binary)
		{
			//TODO: Add option for binary files that aren't images.
			stbi_set_flip_vertically_on_load(true);
			ImageFileInfo file;

			unsigned char *data = 
				stbi_load(path.c_str(), &file.width, &file.height, 
					&file.nrChannels, STBI_rgb_alpha);

			file.Path_ = path;
			std::copy(data, data + (file.width * file.height * file.nrChannels), file.Contents);
			file.LastWriteTime = getLastWriteTime(path);
			//TODO: Add option to change forceNrChannels.
			
			result = std::make_shared<FileInfo>(new FileInfo(dynamic_cast<FileInfo &>(file)));
		}

		return result;
	}
	
	//TODO: Add file to flag file extensions as either text or binary.
	FileType Manager::getFileType(const Path &path)
	{
		static const std::unordered_set<std::string> textExts =
		{
			".txt",
			".vert",
			".frag",
			".comp",
		};

		static const std::unordered_set<std::string> binaryExts =
		{
			".png",
			".jpg",
			".jpeg",
			".bmp",
		};

		auto ext = std::filesystem::path(path).extension().string();
		if(binaryExts.find(ext) != binaryExts.end()) { return Binary; }
		else if(textExts.find(ext) != textExts.end()) { return Text; }
		else { return Text; }
	}


	void Manager::update()
	{
		for (auto &filePair : loadedFiles)
		{
			auto &file = filePair.second;
			auto lastWriteTime = getLastWriteTime(file->Path_);
 			if (file->LastWriteTime == lastWriteTime) { continue; }

			file = loadFile(file->Path_);

			// If the file changed, check for all resources if this file is a dependency 
			// and they need to be reinitialized.
			for(auto &resource : loadedResources)
			{
				if(resource.expired()) { continue; }
				ResourceRef<ResourceBase> ref = std::shared_ptr<ResourceBase>(resource);
				ref->onFileChanged(file->Path_);	
			}
		}
	}
}