# Unreal Engine Utility AI

This is a plugin which aims to implement a utility AI system in Unreal Engine 4, replacing the built-in behavior tree system. It uses some of the classes from the default Behavior Tree system, but also implements a number of new ones.

## What is Utility AI?

See the following GDC talks:

1. https://www.gdcvault.com/play/1012410/Improving-AI-Decision-Modeling-Through

2. https://www.gdcvault.com/play/1021848/Building-a-Better-Centaur-AI

## How does the Unreal Engine Utility AI work?

There are a few components which make the AI tick:

* `UtilityIntelligence`: A component you add to your AI to enable the Utility AI system. Contains `DecisionMakers` and `SkillSets`. Inform the intelligence whenever your AI detects an enemy or friendly, and give your AI any interesting locations it might want to check out (patrol points or the last place it saw an enemy).

* `DecisionMaker` and `SkillSet`: These two classes are somewhat similar, but have different purposes. A `DecisionMaker` is intended for long(ish)-term planning -- should I run away? Should I try to get closer to an enemy? How is my health doing? A `SkillSet` handles any *short-term* actions -- should I shoot my gun? Do I throw a grenade? Should I do a melee attack? The `DecisionMaker` is a list of `Decisions`, whereas a `SkillSet` is a map matching `Skills` to `SkillDecisionScoreEvaluators`.

* A `DecisionScoreEvaluator` (or **DSE**) evaluates scores to determine whether we should make a `Decision`. You can have `SkillDecisionScoreEvaluators`, which give scores to individual `Skills` as well as `NonSkillDecisionScoreEvaluators`, which give scores to `Decisions`. The DSE gets a "starting" weight (generally 1) and a bonus factor (generally 0). The weight and bonus factor get added together and then sent through a bunch of `Considerations`. Each `Consideration` is queried and returns a score between 0 and 1. Our current score is multiplied by the score provided by each `Consideration`, and the final result gets returned. The `DecisionMaker` or `SkillSet` then selects whatever DSE scores the highest.

* A `Consideration`, as mentioned, simply looks at what's going on in a game and provides a value between 0 and 1, with 0 meaning "absolutely do not do this under any circumstances". One `Consideration` can look at which enemies are nearby and give a high score to a melee attack, for example, but then the next `Consideration` can look and see that your character is on extremely low health and shouldn't even *think* about getting close and attacking.

* A `Decision` gets fired a few times a second (*not* every frame). As mentioned, a `Decision` is involved in long-term planning. An example `Decision` would select a target from an array stored on the AI and then try to move close to that target. You can make multiple `Decisions` inside of a `Decision` if needed; the example `CombatDecision` class takes an array of potential enemies and runs an extra DSE on top of the "normal" DSE it's associated with to make a choice about which enemy to attack. In that example, the "normal" DSE should just look at the character's health and the number of enemies, whereas the "combat" DSE should look at enemy health and distance. Note that `Decisions` can't be created at runtime; `CombatDecision` condenses each potential target down to its bare context and then decides based on the raw contexts.

* A `Skill` is very similar to a `Decision`. The main difference is that a `Decision` should *always* use the same DSE, whereas `Skills` may need different DSEs for different characters. For example, a generic "sword attack" skill might mean different things to a Ninja (which should prioritize attacking from behind) vs. a Pirate (which doesn't really care where he is, only that the sword can hit). The `Skill` can be reused, but the `SkillSet` maps it to a different `SkillDecisionScoreEvaluator` based on desired character behavior. Like a `Decision`, a `Skill` also only gets run a few times a second -- generally speaking, it gets run more than `Decisions` do, but still not every frame. You can specify the tick rate for `Decisions` and `Skills` in your `UtilityIntelligence`.

## What to Override

Generally, you should create custom:

* `Considerations`, to give a score based on your game mechanics (health levels, amount of enemies, etc.).

* `Decisions`, to make your AI take custom actions.

* `Skills`, which implement any behaviors you want your AI to have (shooting, for example).

You can also create custom `DecisionScoreEvaluators` if you wanted to play with the concept of "momentum" -- making a decision more or less likely to be chosen if it was chosen recently. However, `Considerations` and `DecisionScoreEvaluators` shouldn't implement any gameplay behavior -- save those for your `Decisions` and `Skills`.

## Dynamic Behaviors

Each `DecisionMaker` contains a set of related `Decisions` that a character can make. The nice thing about these is that you can dynamically add and remove `DecisionMakers` whenever you want. For example, if a character walks into a tavern, they should be able to go up to the tavern and ask for a drink, right? You can set it up so as soon as the AI enters a trigger associated with the tavern, a special "tavern" `DecisionMaker` gets added to the list of decisions they can make. This `DecisionMaker` has `Decisions` for going up to the bar and ordering a drink, making drunken emotes, getting in random brawls, etc. It gets treated just like any of the character's "natural" behaviors, allowing the character to do things they should do in a tavern -- but all the underlying logic remains the same, so if the character gets punched they can still use the same combat behavior they would anywhere else in the world. When the AI leaves the tavern, the special tavern `DecisionMaker` gets removed, and the AI goes back to making `Decisions` like it would normally.

`SkillSets` are handled the same way; if a character picks up a new weapon, that weapon can add an additional `SkillSet` to the character's repertoire, allowing them to execute any special combat-related behaviors associated with that weapon if needed. If the character gets disarmed or the weapon is otherwise removed, then they can lose the `SkillSet` associated with the weapon and it would no longer be a valid option if the character got into trouble.


## Getting Started

1. You should start by adding a `UtilityIntelligence` component to your AIController. You can also implement the `GameplayTags` interface on your AIController if you wanted to have `Considerations` "cooldown" based on a GameplayTag, but this is optional. You can add the `UtilityIntelligence` to the actual Pawn instead, if you wish, but bear in mind that to start the actual AI system you're going to need to pass a reference to a valid AI Controller.

2. The next step is to create the actual assets. Everything else in the system is a `DataAsset`, which is a file stored on the disk that contains data. Because these classes are physical files containing data, you should avoid modifying them at runtime -- in C++, all the functions you override are marked as `const`, but the `const` modifier had to be dropped in some places to provide access via Blueprints.

3. Next, you should override the `Consideration`, `Decision`, and `Skill` classes and implement your custom behaviors. You can use either C++ or Blueprint for this. I've noticed that Unreal's Blueprint support for DataAssets is a bit buggy; it works, but use it at your own risk. A few child classes have already been created for you.

4. In the Content Browser, hit the "Add New" button and go down to "Miscellaneous -> Data Asset." There'll be a list of a whole bunch of files -- among them `DecisionMaker`, `SkillSet`, `SkillDecisionScoreEvaluator`, `NonSkillDecisionScoreEvaluator`, as well as the custom child classes you made for `Consideration`, `Decision`, and `Skill`. Go ahead and create your own DataAssets for each one of these; there's a few already provided for you in the Plugin as well, if needed.

5. Once you create each DataAsset, open it up and modify anything you want to modify. Most things should have descriptive names and tooltips; if you find something that's lacking, feel free to open up an issue here on this GitHub and request clarification, or submit a pull request yourself!

6. Add all your default `DecisionMakers` and `Skills` to the `UtilityIntelligence` attached to your AI Controller. Inside the Blueprint (or C++) code for the AI Controller, call `UtilityIntelligence::StartAI` and pass in your AI Controller and Blackboard Component.

7. You're done! You should see your AI executing decisions when you run the game. Note that there's not much in the way of debugging tools; better visualizations for debugging are still a work in progress.
