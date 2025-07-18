// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <AbilitySystemComponent.h>

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)                                                                                                           \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)                                                                                                 \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)                                                                                                               \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)                                                                                                               \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
