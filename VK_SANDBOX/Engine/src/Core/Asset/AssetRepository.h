#pragma once
#include <unordered_map>
#include <string>
#include <memory>

namespace IHCEngine::Core
{
	template<typename T>
	class AssetRepository
	{
	public:

		bool HasAsset(std::string assetName)
		{
			return assetMap.find(assetName) != assetMap.end();
		}

		T* GetAsset(std::string assetName)
		{
			return assetMap.at(assetName).get();
		}

		void AddAsset(std::string assetName, std::unique_ptr<T> asset)
		{
			if (!HasAsset(assetName))
			{
				assetMap[assetName] = std::move(asset);
			}
		}

		void RemoveAsset(std::string assetName)
		{
			assetMap.erase(assetName);
		}

		void ClearAssets()
		{
			assetMap.clear();
		}

		// readonly
		const std::unordered_map<std::string, std::unique_ptr<T>>& GetAssets() const
		{
			return assetMap;
		}

		void LogAssets()
		{
			for (const auto& [key, value] : assetMap)
			{
				std::cout << "Key: " << key << ", Value: " << *value << '\n';
			}
		}

	private:
		std::unordered_map<std::string, std::unique_ptr<T>> assetMap;
	};
}
