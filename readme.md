# Age of Wujin MUD

## MUDs

Multi-user dungeons (MUDs) originated in the 1970s, and were the first real-time multiplayer computer games. They are the ancestors of modern MMORPGs (Massively Multiplayer Online Role-Playing Games).

## Introduction

You control a character, who can move around a world defined by (X, Y) coordinates.

When exploring the world, you can walk in cardinal directions or move to particular coordinates.

You can hunt for monsters, seek out quest objectives, talk to other players, gain loot, buy equipment, learn skills and spells, gain experience, etc. All the things you do in a normal role-playing game.

## How to Connect

Use a MUD client, such as `telnet`, `tintin++`, `ZMUD`, etc. Use the client to connect to the game's IP address and port.

I might make a web-based client when I finish the game.

## Game Mechanics

### Exploration

The default state is Exploration, where you move around the map and look for adventure.

### Loot

#### Currency

#### Equipment

All equipment has weight, which encumbers you.

##### Weapons

Weapons are used to deal physical damage and deflect physical attacks.

Unlike other games, weapons here have reach, and longer weapons have advantages when used against shorter weapons.

##### Armour

Armour provides both a flat reduction to damage taken, as well as the ability to completely deflect attacks.

Unlike other games, heavier armour is almost universally better than lighter armour in combat situations. Also, smaller weapons not specialized for armour penetration are extremely ineffective against heavy armour.

##### Mounts

Mounts only have the effect of increasing your movement speed when exploring the world.

#### Consumables

### Combat

Combat is real-time and pseudo-turn-based.

#### Active Combat Abilities

Each ability has an associated time lag. It will deliver its effect (e.g. deal damage, status effect, etc.), and then prevent you from doing anything for some period of time. This is called "Delay".

If you queue another ability before the previous ability's Delay is over, it will simply be executed as soon as possible when the Delay is over.

Fleeing from combat is possible, but there are some penalties: you allow the opponent to get free attacks, it may fail, and it takes time before it happens.

#### Passive Combat Abilities

An illustrative example of a passive combat ability is the passive melee attacks. These are delivered together with your active combat abilities, but tend to be significantly weaker. An active ability with a higher delay will be accompanied by a larger number of passive melee attacks.

### Abilities

#### Skills and Spells

Spells aren't mechanically any different from skills, but they are separated from skills just for game-flavour reasons.

Some skills and spells can be used outside of combat.
