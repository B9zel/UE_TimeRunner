// Fill out your copyright notice in the Description page of Project Settings.

#include "Interfaces/StateBackInterface.h"

const FStateBackStorage IStateBackInterface::GetWasState() const
{
	return BPGetWasState();
}
