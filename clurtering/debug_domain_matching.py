"""
Debug script to understand domain presence calculation.
Shows which candidates have which domains.
"""

import json
import numpy as np
from pathlib import Path
import sys

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / 'src'))

from candidate_clustering.features.enhanced_feature_engineer import EnhancedFeatureEngineer


def main():
    # Load data
    data_path = Path(__file__).parent / 'data' / 'processed' / 'candidates_normalized_tier2.json'
    with open(data_path, 'r', encoding='utf-8') as f:
        candidates = json.load(f)
    
    print("="*80)
    print("DOMAIN KEYWORD MATCHING ANALYSIS")
    print("="*80)
    
    DOMAIN_KEYWORDS = {
        'frontend': ['react', 'vue', 'angular', 'javascript', 'typescript', 'css', 'html', 'frontend', 'ui', 'next.js', 'svelte'],
        'backend': ['python', 'java', 'go', 'node', 'django', 'fastapi', 'spring', 'backend', 'api', 'rest', 'graphql', 'express', 'flask'],
        'data': ['pandas', 'numpy', 'tensorflow', 'pytorch', 'machine learning', 'data', 'ml', 'scikit', 'keras', 'deep learning'],
        'devops': ['docker', 'kubernetes', 'jenkins', 'terraform', 'ci/cd', 'devops', 'deployment', 'ansible', 'cloud', 'aws', 'azure', 'gcp'],
        'mobile': ['react native', 'flutter', 'swift', 'kotlin', 'ios', 'android', 'mobile'],
        'database': ['sql', 'postgresql', 'mysql', 'mongodb', 'database', 'nosql', 'sqlite', 'sqlalchemy', 'redis', 'dynamodb'],
    }
    
    # Count candidates per domain
    domain_counts = {domain: 0 for domain in DOMAIN_KEYWORDS}
    domain_examples = {domain: [] for domain in DOMAIN_KEYWORDS}
    
    for candidate in candidates:
        skills = candidate['normalized_skills']
        
        for domain, keywords in DOMAIN_KEYWORDS.items():
            matched_skills = [
                skill for skill in skills
                if any(kw in skill.lower() for kw in keywords)
            ]
            
            if matched_skills:
                domain_counts[domain] += 1
                if len(domain_examples[domain]) < 3:
                    domain_examples[domain].append({
                        'id': candidate['candidate_id'][:8],
                        'skills': matched_skills
                    })
    
    print(f"\nTotal candidates: {len(candidates)}")
    print("\nDomain Distribution:")
    print("-" * 80)
    
    for domain in sorted(domain_counts.keys(), key=lambda d: domain_counts[d], reverse=True):
        count = domain_counts[domain]
        pct = (count / len(candidates)) * 100
        print(f"\n{domain.upper()}: {count} candidates ({pct:.1f}%)")
        
        if domain_examples[domain]:
            print("  Examples:")
            for ex in domain_examples[domain]:
                print(f"    - {ex['id']}: {ex['skills'][:3]}")
        else:
            print("  ⚠️ NO MATCHES - Check if keywords match normalized skill names!")
    
    # Find candidates with devops, mobile, or database skills
    print("\n" + "="*80)
    print("CANDIDATES WITH RARE DOMAINS")
    print("="*80)
    
    for domain in ['devops', 'mobile', 'database']:
        print(f"\n{domain.upper()} Keywords: {DOMAIN_KEYWORDS[domain]}")
        
        # Find candidates with ANY skill containing these keywords
        matches = []
        for candidate in candidates:
            for skill in candidate['normalized_skills']:
                if any(kw in skill.lower() for kw in DOMAIN_KEYWORDS[domain]):
                    matches.append({
                        'id': candidate['candidate_id'][:8],
                        'skill': skill,
                        'all_skills': candidate['normalized_skills']
                    })
                    break
        
        if matches:
            print(f"  Found {len(matches)} candidates")
            for m in matches[:5]:
                print(f"    - {m['id']}: matched '{m['skill']}'")
                print(f"      All skills: {m['all_skills'][:5]}...")
        else:
            print(f"  ⚠️ NO CANDIDATES with {domain} skills!")
            print("  Checking raw skill names in data...")
            
            # Find ANY skills that might be related
            all_skills = set()
            for c in candidates:
                all_skills.update(c['normalized_skills'])
            
            # Broader search
            related = [s for s in all_skills if domain[:4] in s.lower()]
            if related:
                print(f"  Skills containing '{domain[:4]}': {related[:10]}")


if __name__ == '__main__':
    main()
