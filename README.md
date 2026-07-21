# Time Runner

Time Runner is a single-player first-person sci-fi action game where time slows
down while the player is moving. The main goal is to explore a facility, collect
fragments of a mysterious artifact, and fight enemies using time slowdown, high
mobility (dashes, wall running), and melee combat.

Slowdown comes at a price: the longer and more aggressively you use it, the
higher your intoxication rises, and intoxication drains your health. Every run
is a balance between using the ability and staying safe. Each artifact fragment
you find unlocks new abilities — but also brings more dangerous enemies and a
higher risk of intoxication.


**Stack:** Unreal Engine 5.7 · C++20 · Gameplay Ability System · StateTree / GameplayStateTree · AIModule · Enhanced Input

## Table of Contents

- [Key Idea](#key-idea)
- [Features](#features)
- [Character Mechanics](#character-mechanics)
- [Enemies](#enemies)
- [Controls](#controls)
- [Build and Run](#build-and-run)
- [Project Structure](#project-structure)
- [Architecture](#architecture)

## Key Idea

The speed of the world is tied to the player's movement. While run is held and
the character is grounded, the time slowdown ability (`Ability.Dilation`)
kicks in, the global `TimeDilation` rises, and the character compensates
through `CustomTimeDilation` to move at its own pace. Releasing run, crouching,
or falling off a ledge resets time back to normal.

On top of that, a second loop runs: the mouse wheel switches between four
world speed levels (`SLOW → MIDDLE → FAST → VERY_FAST`). The higher the level,
the faster the world lives — and the faster **intoxication** rises. Cross the
safe threshold and the character starts taking periodic damage. Fast means
dangerous.

## Features

- Player-input-driven time slowdown with four switchable speed levels.
- Intoxication as the price of speed: rises while slowdown is active, deals
  damage above a threshold.
- Rewind of state — teleport back to a saved position, rotation, and health
  from a ring buffer of snapshots.
- Wall running with side wall tracing, a hold timer, and a jump off the wall.
- Dash via a GAS ability with a cooldown and a once-per-airtime restriction.
- Katana melee combat: capsule trace → gameplay event → damage applied through GAS.
- StateTree-driven AI: patrolling, target search, melee and ranged attacks,
  and an alert system (`Alert`) as a separate attribute set.
- A custom global timer independent of time slowdown.
- A bullet pool owned by the GameMode.

## Character Mechanics

| Component | What it does |
|---|---|
| `UTimeDilationComponent` | Stores speed levels as polymorphic `BaseSpeed` objects, enables/disables the slowdown ability, broadcasts `ChanageSpeedDelegate` on level change |
| `UIntoxicationComponent` | Increase/decrease of intoxication through GAS effects, periodic damage above `SafeBorder`, full set of getters/setters for Blueprint |
| `UCountermotionComponent` | Ring buffer of `FCountermotionData` snapshots (position, rotation, health) at a configurable rate and time depth |
| `URunWallComponent` | Side wall tracing, start/stop wall running, jump off the wall with separate directional and Z forces |
| `UHealthComponent` | Wrapper over `UHealthAttributeSet`, `ChangeHealth` and `Dead` delegates, regen while running and idle |

## Enemies

| Enemy | Type | Highlight |
|---|---|---|
| Melee Cyborg | melee | Close-range attack plus a protective field ability (`GA_DefendYourSelf`, ProtectField) |
| Resonator | melee | Barrier, a repulsive impulse, and a time-pulse sphere (`Ability.Enemy.Resonator.*`) |
| Distorter | ranged | A time-distortion sphere as a projectile zone |
| Bullet Turret | static | Wall and floor variants, fires bullets from a shared pool |

## Controls

Keys are configured in `IMC_Runner`, actions are declared in the `FInput` struct:

| Action | Purpose |
|---|---|
| `RunAction` | Movement; holding it enables time slowdown |
| `LookActions` | Look around |
| `JumpActions` | Jump, start wall running, jump off the wall |
| `SwitchSpeedActions` | Switch world speed level up/down |
| `CrouchActions` | Crouch (resets slowdown) |
| `AttackAction` | Katana strike |
| `DashAction` | Dash |
| `OldestStateAction` | Rewind to the oldest saved state |

## Build and Run

You need Unreal Engine 5.7 and Visual Studio 2022.

```bash
git clone <repo-url>
cd TimeRunner
```

1. Right-click `TimeRunner.uproject` → **Generate Visual Studio project files**.
2. Open `TimeRunner.sln`, set configuration to **Development Editor**, platform **Win64**, and build.
3. Launch the editor — the startup map is `L_MainLevel`, the gameplay level is `L_FirstLevel`.

Code formatting uses clang-format (style defined in `.clang-format`), run over
all sources with:

```bash
Foratting.bat
```

## Project Structure

```
Source/TimeRunner/
├── AbilitySystem/      # ASC, attribute sets, C++ abilities
├── Characters/         # Base character, player, AI characters and controllers
├── Components/         # Mechanics: time, intoxication, wall running, rewind, health
├── Consumable/         # Bullets
├── Data/               # Data assets and config structs
├── Global/             # GameMode, GameInstance, PlayerState, global timer, pools
├── Interfaces/         # Mechanic interfaces (state rewind, wall running, slowdown)
├── StateTree/          # C++ conditions for StateTree
├── StaticEnemies/      # Turrets
└── Weapons/            # Base weapon

Content/Game/
├── Blueprints/         # Abilities, effects, characters, AI, turrets, doors
├── Data/               # DataTables and enums
├── Input/              # Input actions and mapping context
└── Levels/             # L_MainLevel, L_FirstLevel
```

## Architecture

The Ability System lives on `ATRPlayerState`, not on the character — so
abilities survive a pawn change. Mechanic components wait for possession
before reaching for it:

```
Input (Enhanced Input)
        │
        ▼
ATimerRunnerCharacter ──► mechanic interfaces (ITimeDilationInterface, IWallRunInterface, IStateBackInterface)
        │                          │
        │                          ▼
        │                 U*Component (time / intoxication / wall / rewind / health)
        │                          │
        ▼                          ▼
ATRPlayerState ──► UTRAbilitySystemComponent ──► GameplayAbility ──► GameplayEffect
                                  │
                                  ▼
                          U*AttributeSet ──► delegates ──► back to components and UI
```

| Class | Responsibility |
|---|---|
| `ATimerRunnerCharacter` | Input, wiring mechanics together, implementing interfaces; actual logic lives in components |
| `ATRPlayerState` | Owner of `UTRAbilitySystemComponent` — survives character respawn |
| `UTRAbilitySystemComponent` | Tag-based ability activation with returned handles, tag-based cancellation, `OnEndAbility` delegate |
| `UBaseAttributeSet` | Shared attribute clamping and change delegate for subclasses (Health, Intoxication, Damage, Alert) |
| `UGlobalTimer` | Timers on real time: `FPlatformTime::Seconds()` instead of game time, since time slowdown would otherwise break all periodic logic |
| `UBulletPool` | Reuses bullets instead of spawning/destroying; created by `ABaseGameMode` |
