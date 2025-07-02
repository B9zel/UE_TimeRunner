#pragma once

#if (AUTOMATION_TEST)

#include "CoreMinimal.h"
#include <Tests/AutomationCommon.h>

namespace AutomationTestUtils
{

const FString NameTestMap = "/Game/Tests/Level/L_EmptyTest";
const FString BPTimeRunnerName = "/Game/Game/Blueprints/Characters/TimeRunner/BP_TimeRunnerCharacter.BP_TimeRunnerCharacter";

class OpenAutomationMap
{
public:

	OpenAutomationMap(const FString& Map)
	{
		AutomationOpenMap(Map);
	}
	~OpenAutomationMap()
	{
		ADD_LATENT_AUTOMATION_COMMAND(FExitGameCommand);
	}
};

} // namespace AutomationTestUtils

#endif