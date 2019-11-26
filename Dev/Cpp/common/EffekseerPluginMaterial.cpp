#include "EffekseerPluginMaterial.h"
#include <algorithm>

namespace EffekseerPlugin
{

MaterialLoader::MaterialLoader(MaterialLoaderLoad load, MaterialLoaderUnload unload)
	: load(load), unload(unload), memoryFile_(1 * 1024 * 1024), memoryFileForCache_(1 * 1024 * 1024)
{
}

Effekseer::MaterialData* MaterialLoader::Load(const EFK_CHAR* path)
{
	auto it = resources.find((const char16_t*)path);
	if (it != resources.end())
	{
		it->second.referenceCount++;
		return it->second.internalData;
	}

	// Load with unity
	MaterialResource res;
	int requiredDataSize = 0;
	int requiredCachedDataSize = 0;

	auto materialPtr = load((const char16_t*)path,
						 &memoryFile_.LoadedBuffer[0],
						 (int)memoryFile_.LoadedBuffer.size(),
						 requiredDataSize,
						 &memoryFileForCache_.LoadedBuffer[0],
						 (int)memoryFileForCache_.LoadedBuffer.size(),
						 requiredCachedDataSize);

	if (requiredDataSize == 0 && requiredCachedDataSize == 0)
	{
		// Failed to load
		return nullptr;
	}

	if (materialPtr == nullptr)
	{
		// Lack of memory
		memoryFileForCache_.Resize(requiredCachedDataSize);
		memoryFile_.Resize(requiredDataSize);

		// Load with unity
		materialPtr = load((const char16_t*)path,
						   &memoryFile_.LoadedBuffer[0],
						   (int)memoryFile_.LoadedBuffer.size(),
						   requiredDataSize,
						   &memoryFileForCache_.LoadedBuffer[0],
						   (int)memoryFileForCache_.LoadedBuffer.size(),
						   requiredCachedDataSize);

		if (materialPtr == nullptr)
		{
			// Failed to load
			return nullptr;
		}
	}

	// 内部ローダに渡してロード処理する
	memoryFile.LoadedSize = (size_t)requiredDataSize;
	res.internalData = internalLoader->Load(path);

	resources.insert(std::make_pair((const char16_t*)path, res));
	return res.internalData;
}
void MaterialLoader::Unload(Effekseer::MaterialData* data)
{
	if (data == nullptr)
	{
		return;
	}

	auto it = std::find_if(resources.begin(), resources.end(), [data](const std::pair<std::u16string, MaterialResource>& pair) {
		return pair.second.internalData == data;
	});
	if (it == resources.end())
	{
		return;
	}

	it->second.referenceCount--;
	if (it->second.referenceCount <= 0)
	{
		internalLoader->Unload(it->second.internalData);
		unload(it->first.c_str(), nullptr);
		resources.erase(it);
	}
}
} // namespace EffekseerPlugin