# Unreal Engine Utility AI

This is a plugin which aims to implement a utility AI system in Unreal Engine 4, replacing the built-in behavior tree system.

Essentially, it works like so:

* `UtilityIntelligence`: A component you add to your AI Controller to enable the system. Contains `DecisionMakers` and `SkillSets`. Inform the intelligence whenever your AI detects an enemy or friendly, and give your AI any interesting locations it might want to check out (patrol points or the last place it saw an enemy).

* `DecisionMaker` and `SkillSet`: These two classes are somewhat similar, but have different purposes.

For more information, see the following GDC talks:

1. https://www.gdcvault.com/play/1012410/Improving-AI-Decision-Modeling-Through

2. https://www.gdcvault.com/play/1021848/Building-a-Better-Centaur-AI