#include "BsEngineScriptLibrary.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsScriptAssemblyManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsManagedResourceManager.h"
#include "BsScriptManager.h"
#include "BsScriptInput.h"
#include "BsScriptVirtualInput.h"
#include "BsScriptObjectManager.h"
#include "BsGameResourceManager.h"
#include "BsApplication.h"
#include "BsFileSystem.h"
#include "BsScriptDebug.h"

namespace BansheeEngine
{
	EngineScriptLibrary::EngineScriptLibrary()
		:mScriptAssembliesLoaded(false)
	{ }

	void EngineScriptLibrary::initialize()
	{
		Path engineAssemblyPath = gApplication().getEngineAssemblyPath();
		const String ASSEMBLY_ENTRY_POINT = "Program::Start";

		MonoManager::startUp();
		MonoAssembly& bansheeEngineAssembly = MonoManager::instance().loadAssembly(engineAssemblyPath.toString(), ENGINE_ASSEMBLY);

		ScriptDebug::startUp();
		GameResourceManager::startUp();
		ScriptObjectManager::startUp();
		ManagedResourceManager::startUp();
		ScriptAssemblyManager::startUp();
		ScriptResourceManager::startUp();
		ScriptGameObjectManager::startUp();
		ScriptInput::startUp();
		ScriptVirtualInput::startUp();

		ScriptAssemblyManager::instance().loadAssemblyInfo(ENGINE_ASSEMBLY);
		bansheeEngineAssembly.invoke(ASSEMBLY_ENTRY_POINT);
	}

	void EngineScriptLibrary::reload()
	{
		Path engineAssemblyPath = gApplication().getEngineAssemblyPath();
		Path gameAssemblyPath = gApplication().getGameAssemblyPath();

		// Do a full refresh if we have already loaded script assemblies
		if (mScriptAssembliesLoaded)
		{
			Vector<std::pair<String, Path>> assemblies;
			assemblies.push_back({ ENGINE_ASSEMBLY, engineAssemblyPath });

			if (FileSystem::exists(gameAssemblyPath))
				assemblies.push_back({ SCRIPT_GAME_ASSEMBLY, gameAssemblyPath });

			ScriptObjectManager::instance().refreshAssemblies(assemblies);
		}
		else // Otherwise just additively load them
		{
			MonoManager::instance().loadAssembly(engineAssemblyPath.toString(), ENGINE_ASSEMBLY);
			ScriptAssemblyManager::instance().loadAssemblyInfo(ENGINE_ASSEMBLY);

			if (FileSystem::exists(gameAssemblyPath))
			{
				MonoManager::instance().loadAssembly(gameAssemblyPath.toString(), SCRIPT_GAME_ASSEMBLY);
				ScriptAssemblyManager::instance().loadAssemblyInfo(SCRIPT_GAME_ASSEMBLY);
			}

			mScriptAssembliesLoaded = true;
		}
	}

	void EngineScriptLibrary::destroy()
	{
		unloadAssemblies();
		shutdownModules();
	}

	void EngineScriptLibrary::unloadAssemblies()
	{
		ManagedResourceManager::instance().clear();
		MonoManager::instance().unloadScriptDomain();
		ScriptObjectManager::instance().processFinalizedObjects();
	}

	void EngineScriptLibrary::shutdownModules()
	{
		ScriptVirtualInput::shutDown();
		ScriptInput::shutDown();
		ManagedResourceManager::shutDown();
		MonoManager::shutDown();
		ScriptGameObjectManager::shutDown();
		ScriptResourceManager::shutDown();
		ScriptAssemblyManager::shutDown();
		ScriptObjectManager::shutDown();
		GameResourceManager::shutDown();
		ScriptDebug::shutDown();
	}
}