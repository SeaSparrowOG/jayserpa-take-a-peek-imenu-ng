#include "PeekMenu.h"
#include "Papyrus.h"


const SKSE::MessagingInterface* g_messaging = nullptr;

static void SKSEMessageHandler(SKSE::MessagingInterface::Message* message)
{
	switch (message->type) {
	case SKSE::MessagingInterface::kDataLoaded:
		CustomPeekMenu::Register();
		break;

	case SKSE::MessagingInterface::kNewGame:
		//CustomPeekMenu::Show();
		break;

	case SKSE::MessagingInterface::kPostLoadGame:
		//CustomPeekMenu::Show();
		break;
	}
}

void InitializeLogging() {
	auto path = SKSE::log::log_directory();
	if (!path) {
		SKSE::stl::report_and_fail("Unable to lookup SKSE logs directory.");
	}
	*path /= SKSE::PluginDeclaration::GetSingleton()->GetName();
	*path += L".log";

	std::shared_ptr<spdlog::logger> log;
	if (IsDebuggerPresent()) {
		log = std::make_shared<spdlog::logger>(
			"Global", std::make_shared<spdlog::sinks::msvc_sink_mt>());
	}
	else {
		log = std::make_shared<spdlog::logger>(
			"Global", std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true));
	}
	log->set_level(spdlog::level::info);
	log->flush_on(spdlog::level::info);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%H:%M:%S.%e]: %v");
}

extern "C" DLLEXPORT bool SKSEAPI SKSEPlugin_Load(const SKSE::LoadInterface * a_skse) {

	InitializeLogging();
	SKSE::log::info("Plugin loading...");

	SKSE::Init(a_skse);

	const auto papyrus = SKSE::GetPapyrusInterface();
	papyrus->Register(Papyrus::Bind);

	g_messaging = SKSE::GetMessagingInterface();
	if (!g_messaging) {

		SKSE::log::info("Failed to load messaging interface! This error is fatal, plugin will not load.");
		return false;
	}
	g_messaging->RegisterListener("SKSE", SKSEMessageHandler);


	return true;
}