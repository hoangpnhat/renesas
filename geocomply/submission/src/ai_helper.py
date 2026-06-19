"""
AI/LLM Helper for location spoofing detection.
Provides rule proposal, weak labeling, and explanation capabilities.

Note: This module includes both prompts and mocked implementations
since we don't have access to external LLM APIs in this environment.
"""

import pandas as pd
import numpy as np
from typing import List, Dict
import json
import os


class AIHelper:
    """
    AI-augmented helper for spoofing detection.
    Includes: Rule Proposer, Weak Labeler, and Explainer.
    """

    def __init__(self, use_mock: bool = True):
        """
        Initialize AI Helper.

        Args:
            use_mock: If True, use deterministic mocked responses.
                     If False, would call external LLM API (not implemented).
        """
        self.use_mock = use_mock

    # ========== RULE PROPOSER ==========

    def get_rule_proposer_prompt(self, schema_desc: str, examples: str) -> str:
        """
        Generate prompt for AI to propose detection rules.

        This prompt would be sent to an LLM API (e.g., Claude, GPT-4)
        to get rule suggestions based on the data schema and examples.
        """
        prompt = f"""You are an expert in mobile location spoofing detection. Given the following SDK data schema and examples, propose interpretable detection rules.

SCHEMA:
{schema_desc}

EXAMPLES:
{examples}

TASK:
Propose 5 interpretable rules for detecting location spoofing. For each rule:
1. Provide a clear name
2. Describe the detection logic
3. Suggest threshold values
4. Note potential false positive scenarios

Format your response as JSON:
[
  {{
    "name": "rule_name",
    "description": "what it detects",
    "logic": "how to implement it",
    "threshold": "suggested threshold values",
    "false_positive_risks": "scenarios where legitimate users might trigger this"
  }},
  ...
]
"""
        return prompt

    def propose_rules_mocked(self) -> List[Dict]:
        """
        Mocked version of rule proposer.
        Returns rules that an LLM might suggest given the schema.
        """
        proposed_rules = [
            {
                "name": "impossible_speed",
                "description": "Detects teleportation via impossible travel speed",
                "logic": "Calculate speed between consecutive GPS points; flag if > 150 km/h",
                "threshold": "150 km/h for normal users, 500 km/h even for aircraft",
                "false_positive_risks": "Users near airports, GPS glitches during tunnels"
            },
            {
                "name": "ip_geo_mismatch",
                "description": "Detects when IP geolocation doesn't match GPS coordinates",
                "logic": "Compare IP region with GPS coordinates; flag if > 5 degrees apart",
                "threshold": "5 degrees latitude/longitude difference",
                "false_positive_risks": "Users with VPNs, users near region boundaries"
            },
            {
                "name": "mock_location_indicator",
                "description": "Directly detects mock location settings",
                "logic": "Check if mock_location_enabled flag is True",
                "threshold": "Boolean flag",
                "false_positive_risks": "Developers testing apps, legitimate testing scenarios"
            },
            {
                "name": "sensor_movement_inconsistency",
                "description": "Detects movement without corresponding sensor activity",
                "logic": "If speed > 1 m/s but accelerometer_variance < 0.01, flag as suspicious",
                "threshold": "speed > 1 m/s, accel_var < 0.01",
                "false_positive_risks": "Device on steady vehicle (train, plane), sensor calibration issues"
            },
            {
                "name": "perfect_accuracy_anomaly",
                "description": "Detects suspiciously perfect GPS accuracy",
                "logic": "Real GPS has natural variance; accuracy < 2m is suspicious",
                "threshold": "accuracy < 2 meters",
                "false_positive_risks": "High-end devices in open areas, stationary devices with clear sky view"
            }
        ]
        return proposed_rules

    def propose_rules(self, schema_desc: str = None, examples: str = None) -> List[Dict]:
        """
        Propose detection rules using AI.

        In production, this would call an LLM API with the prompt.
        For this demo, we use mocked responses.
        """
        if self.use_mock:
            return self.propose_rules_mocked()
        else:
            # In production: call external LLM API with the prompt
            prompt = self.get_rule_proposer_prompt(schema_desc, examples)
            # response = llm_api_call(prompt)
            # return parse_llm_response(response)
            raise NotImplementedError("External LLM API not configured")

    # ========== WEAK LABELER ==========

    def get_weak_labeler_prompt(self, event_note: str, event_features: Dict) -> str:
        """
        Generate prompt for AI to create weak labels from textual notes.

        This would help label data when we have descriptive notes but unclear labels.
        """
        prompt = f"""You are analyzing a mobile location event for potential spoofing.

EVENT NOTE: "{event_note}"

EVENT FEATURES:
{json.dumps(event_features, indent=2)}

TASK:
Based on the note and features, determine if this event is likely spoofed or legitimate.

Respond with JSON:
{{
  "prediction": "spoofed" or "legitimate",
  "confidence": 0.0 to 1.0,
  "reasoning": "brief explanation"
}}
"""
        return prompt

    def weak_label_mocked(self, event_note: str, event_features: Dict) -> Dict:
        """
        Mocked weak labeler.
        Uses deterministic rules to simulate what an LLM would infer.
        """
        note_lower = event_note.lower()

        # Check for spoofing keywords in note
        spoof_keywords = ['teleport', 'ip mismatch', 'mock', 'timezone', 'sensor mismatch', 'spoof']
        legit_keywords = ['legitimate', 'real', 'actual', 'genuine']

        spoof_score = sum(1 for kw in spoof_keywords if kw in note_lower)
        legit_score = sum(1 for kw in legit_keywords if kw in note_lower)

        # Check features
        if event_features.get('mock_location_enabled', False):
            spoof_score += 2

        if event_features.get('accuracy', 10) < 2:
            spoof_score += 1

        # Determine prediction
        if spoof_score > legit_score:
            return {
                "prediction": "spoofed",
                "confidence": min(0.5 + spoof_score * 0.1, 0.95),
                "reasoning": f"Note contains spoofing indicators: {event_note[:50]}..."
            }
        else:
            return {
                "prediction": "legitimate",
                "confidence": 0.7,
                "reasoning": "No strong spoofing indicators in note or features"
            }

    def weak_label(self, event_note: str, event_features: Dict) -> Dict:
        """
        Create weak label from event note using AI.

        In production, this would call an LLM API.
        For this demo, we use mocked logic.
        """
        if self.use_mock:
            return self.weak_label_mocked(event_note, event_features)
        else:
            prompt = self.get_weak_labeler_prompt(event_note, event_features)
            # response = llm_api_call(prompt)
            # return parse_llm_response(response)
            raise NotImplementedError("External LLM API not configured")

    # ========== EXPLAINER ==========

    def get_explainer_prompt(self, event_features: Dict, triggered_rules: List[str],
                            ml_score: float) -> str:
        """
        Generate prompt for AI to explain why an event was flagged.

        This creates human-readable explanations for reviewers.
        """
        prompt = f"""You are explaining why a location event was flagged as potentially spoofed.

EVENT FEATURES:
{json.dumps(event_features, indent=2)}

TRIGGERED RULES:
{', '.join(triggered_rules) if triggered_rules else 'None'}

ML SPOOFING SCORE: {ml_score:.3f}

TASK:
Provide a clear, concise explanation (2-3 sentences) that a human reviewer can understand.
Focus on the most suspicious aspects and explain in plain language.

Format:
"This event was flagged because [main reason]. Additionally, [supporting evidence]. Confidence: [high/medium/low]."
"""
        return prompt

    def explain_mocked(self, event_features: Dict, triggered_rules: List[str],
                      ml_score: float) -> str:
        """
        Mocked explainer.
        Generates deterministic explanations based on rules and score.
        """
        explanations = []

        # Analyze triggered rules
        if 'rule_teleportation' in triggered_rules:
            explanations.append("the device appears to have teleported to a distant location impossibly fast")

        if 'rule_ip_geo_mismatch' in triggered_rules:
            explanations.append("the IP address location doesn't match the GPS coordinates")

        if 'rule_mock_location' in triggered_rules:
            explanations.append("mock location/GPS spoofing software is enabled on the device")

        if 'rule_sensor_mismatch' in triggered_rules:
            explanations.append("the device is moving but motion sensors show no activity")

        if 'rule_timezone_mismatch' in triggered_rules:
            explanations.append("the device timezone doesn't match the GPS location")

        if 'rule_perfect_accuracy' in triggered_rules:
            explanations.append("the GPS accuracy is suspiciously perfect (< 2m)")

        # Determine confidence
        if ml_score > 0.8 or len(triggered_rules) >= 3:
            confidence = "high"
        elif ml_score > 0.5 or len(triggered_rules) >= 2:
            confidence = "medium"
        else:
            confidence = "low"

        # Build explanation
        if not explanations:
            return f"This event shows subtle spoofing indicators. ML model confidence: {ml_score:.1%}. Manual review recommended. Confidence: {confidence}."

        main_reason = explanations[0]
        if len(explanations) > 1:
            supporting = " Additionally, " + " and ".join(explanations[1:])
        else:
            supporting = ""

        return f"This event was flagged because {main_reason}.{supporting} ML confidence: {ml_score:.1%}. Overall confidence: {confidence}."

    def explain(self, event_features: Dict, triggered_rules: List[str],
               ml_score: float) -> str:
        """
        Generate natural language explanation for a flagged event.

        In production, this would call an LLM API.
        For this demo, we use mocked logic.
        """
        if self.use_mock:
            return self.explain_mocked(event_features, triggered_rules, ml_score)
        else:
            prompt = self.get_explainer_prompt(event_features, triggered_rules, ml_score)
            # response = llm_api_call(prompt)
            # return response
            raise NotImplementedError("External LLM API not configured")


def demonstrate_ai_helper():
    """
    Demonstrate AI helper capabilities.
    """
    print("=" * 60)
    print("AI Helper Demonstration")
    print("=" * 60)

    ai_helper = AIHelper(use_mock=True)

    # 1. Rule Proposer
    print("\n[1] RULE PROPOSER")
    print("-" * 60)
    print("Proposing detection rules based on schema...")

    proposed_rules = ai_helper.propose_rules()
    print(f"\nProposed {len(proposed_rules)} rules:")
    for i, rule in enumerate(proposed_rules, 1):
        print(f"\n{i}. {rule['name'].upper()}")
        print(f"   Description: {rule['description']}")
        print(f"   Logic: {rule['logic']}")
        print(f"   Threshold: {rule['threshold']}")
        print(f"   FP Risks: {rule['false_positive_risks']}")

    # 2. Weak Labeler
    print("\n\n[2] WEAK LABELER")
    print("-" * 60)
    print("Creating weak labels from textual notes...")

    test_notes = [
        ("teleported to random location", {'mock_location_enabled': False, 'accuracy': 5.0}),
        ("normal movement pattern", {'mock_location_enabled': False, 'accuracy': 8.0}),
        ("mock location app detected", {'mock_location_enabled': True, 'accuracy': 1.0}),
    ]

    for note, features in test_notes:
        result = ai_helper.weak_label(note, features)
        print(f"\nNote: '{note}'")
        print(f"  Prediction: {result['prediction']}")
        print(f"  Confidence: {result['confidence']:.2f}")
        print(f"  Reasoning: {result['reasoning']}")

    # 3. Explainer
    print("\n\n[3] EXPLAINER")
    print("-" * 60)
    print("Generating explanations for flagged events...")

    test_cases = [
        {
            'features': {
                'event_id': 'evt_001234',
                'latitude': 37.7749,
                'longitude': -122.4194,
                'accuracy': 1.0,
                'mock_location_enabled': True
            },
            'rules': ['rule_mock_location', 'rule_perfect_accuracy'],
            'ml_score': 0.95
        },
        {
            'features': {
                'event_id': 'evt_005678',
                'latitude': 40.7128,
                'longitude': -74.0060,
                'speed': 25.0,
                'accelerometer_variance': 0.005
            },
            'rules': ['rule_sensor_mismatch'],
            'ml_score': 0.62
        }
    ]

    for case in test_cases:
        explanation = ai_helper.explain(
            case['features'],
            case['rules'],
            case['ml_score']
        )
        print(f"\nEvent: {case['features']['event_id']}")
        print(f"Explanation: {explanation}")

    print("\n" + "=" * 60)
    print("AI Helper demonstration complete!")
    print("=" * 60)


def main():
    """Run AI helper demonstration."""
    demonstrate_ai_helper()

    # Show prompts that would be used with real LLM API
    print("\n\n" + "=" * 60)
    print("EXAMPLE PROMPTS FOR REAL LLM API")
    print("=" * 60)

    ai_helper = AIHelper(use_mock=True)

    print("\n[RULE PROPOSER PROMPT]")
    print("-" * 60)
    schema_desc = """
    - latitude, longitude: GPS coordinates
    - accuracy: GPS accuracy in meters
    - speed: Device speed in m/s
    - mock_location_enabled: Boolean flag
    - accelerometer_variance: Motion sensor variance
    """
    examples = """
    Spoofed: lat=37.7, lon=-122.4, accuracy=1.0, mock_enabled=True
    Legitimate: lat=37.7, lon=-122.4, accuracy=8.5, mock_enabled=False
    """
    print(ai_helper.get_rule_proposer_prompt(schema_desc, examples))

    print("\n\n[WEAK LABELER PROMPT]")
    print("-" * 60)
    event_note = "teleported to random location"
    event_features = {'accuracy': 1.0, 'mock_location_enabled': True}
    print(ai_helper.get_weak_labeler_prompt(event_note, event_features))

    print("\n\n[EXPLAINER PROMPT]")
    print("-" * 60)
    features = {'event_id': 'evt_001', 'latitude': 37.7, 'accuracy': 1.0}
    rules = ['rule_mock_location', 'rule_perfect_accuracy']
    print(ai_helper.get_explainer_prompt(features, rules, 0.95))


if __name__ == "__main__":
    main()
