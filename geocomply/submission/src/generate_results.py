"""
Generate final results.json for test set.
Combines rules-based and ML predictions with AI explanations.
"""

import pandas as pd
import json
import os
from ai_helper import AIHelper


def generate_final_results():
    """
    Generate results.json with predictions and explanations for test set.
    """
    print("=" * 60)
    print("Generating Final Results")
    print("=" * 60)

    # Setup paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(os.path.dirname(script_dir), 'data')

    # Load test predictions
    print("\nLoading test predictions...")
    test_df = pd.read_csv(os.path.join(data_dir, 'test.csv'))
    rules_results = pd.read_csv(os.path.join(data_dir, 'test_rules_results.csv'))
    ml_results = pd.read_csv(os.path.join(data_dir, 'test_ml_results.csv'))

    print(f"Loaded {len(test_df)} test events")

    # Initialize AI helper for explanations
    ai_helper = AIHelper(use_mock=True)

    # Generate combined results
    results = []

    print("\nGenerating predictions and explanations...")
    for idx in range(len(test_df)):
        event_id = test_df.loc[idx, 'event_id']

        # Get scores from both approaches
        rules_score = rules_results.loc[idx, 'spoof_score_rules']
        ml_score = ml_results.loc[idx, 'spoof_score_ml']

        # Combined score (weighted average: 60% rules, 40% ML)
        # Rules perform better overall, so give them more weight
        combined_score = 0.6 * rules_score + 0.4 * ml_score

        # Final decision: flag if combined score > 0.5 OR either method flags with high confidence
        spoof_flag_rules = rules_results.loc[idx, 'spoof_flag_rules']
        spoof_flag_ml = ml_results.loc[idx, 'spoof_flag_ml']

        # Use hybrid approach: flag if rules flag it (since rules have better recall)
        spoof_flag = bool(spoof_flag_rules)

        # Get triggered rules for explanation
        triggered_rules = []
        rule_columns = [col for col in rules_results.columns if col.startswith('rule_') and col != 'spoof_score_rules' and col != 'spoof_flag_rules']
        for rule_col in rule_columns:
            if rules_results.loc[idx, rule_col]:
                triggered_rules.append(rule_col)

        # Generate explanation if flagged
        if spoof_flag:
            # Extract key features for explanation
            event_features = {
                'event_id': event_id,
                'latitude': test_df.loc[idx, 'latitude'],
                'longitude': test_df.loc[idx, 'longitude'],
                'accuracy': test_df.loc[idx, 'accuracy'],
                'speed': test_df.loc[idx, 'speed'],
                'mock_location_enabled': test_df.loc[idx, 'mock_location_enabled'],
                'accelerometer_variance': test_df.loc[idx, 'accelerometer_variance'],
            }

            explanation = ai_helper.explain(event_features, triggered_rules, combined_score)
        else:
            explanation = None

        # Add to results
        result = {
            'event_id': event_id,
            'spoof_score': round(float(combined_score), 4),
            'spoof_flag': spoof_flag,
            'explanation': explanation
        }

        results.append(result)

        # Progress indicator
        if (idx + 1) % 1000 == 0:
            print(f"  Processed {idx + 1}/{len(test_df)} events...")

    print(f"  Processed {len(results)}/{len(test_df)} events")

    # Summary statistics
    num_flagged = sum(1 for r in results if r['spoof_flag'])
    print(f"\nSummary:")
    print(f"  Total events: {len(results)}")
    print(f"  Flagged as spoof: {num_flagged} ({num_flagged/len(results)*100:.1f}%)")
    print(f"  Legitimate: {len(results) - num_flagged} ({(len(results)-num_flagged)/len(results)*100:.1f}%)")

    # Save results
    output_path = os.path.join(os.path.dirname(data_dir), 'results.json')
    with open(output_path, 'w') as f:
        json.dump(results, f, indent=2)

    print(f"\n[OK] Results saved to {output_path}")

    # Show a few example results
    print("\n" + "-" * 60)
    print("Example Results:")
    print("-" * 60)

    # Show 3 flagged examples
    flagged_examples = [r for r in results if r['spoof_flag']][:3]
    for i, example in enumerate(flagged_examples, 1):
        print(f"\n{i}. Event {example['event_id']} (FLAGGED)")
        print(f"   Score: {example['spoof_score']:.4f}")
        print(f"   Explanation: {example['explanation']}")

    # Show 1 legitimate example
    legit_examples = [r for r in results if not r['spoof_flag']][:1]
    for example in legit_examples:
        print(f"\n4. Event {example['event_id']} (LEGITIMATE)")
        print(f"   Score: {example['spoof_score']:.4f}")
        print(f"   Explanation: {example['explanation']}")

    print("\n" + "=" * 60)
    print("Results generation complete!")
    print("=" * 60)


if __name__ == "__main__":
    generate_final_results()
