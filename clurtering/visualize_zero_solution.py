"""
Visualization showing the solution to "too many zeros" problem.
Creates a before/after comparison chart.
"""

import json
import matplotlib.pyplot as plt
import numpy as np
from pathlib import Path
import sys

# Add src to path
sys.path.insert(0, str(Path(__file__).parent / 'src'))

from candidate_clustering.features.feature_engineer import FeatureEngineer
from candidate_clustering.features.enhanced_feature_engineer import EnhancedFeatureEngineer


def visualize_zero_problem():
    """Create visual comparison of old vs new approach."""
    
    # Load data
    data_path = Path(__file__).parent / 'data' / 'processed' / 'candidates_normalized_tier2.json'
    with open(data_path, 'r', encoding='utf-8') as f:
        candidates = json.load(f)
    
    # Select 3 diverse candidates
    # 1. Backend specialist with database skills
    backend_db = None
    # 2. Frontend + DevOps (full-stack with ops)
    fullstack_devops = None
    # 3. Mobile developer (rare)
    mobile_dev = None
    
    for c in candidates:
        skills = c['normalized_skills']
        n_skills = len(skills)
        
        # Backend + Database specialist
        if not backend_db:
            has_backend = any(kw in str(skills).lower() for kw in ['python', 'java', 'backend', 'api'])
            has_database = any(kw in str(skills).lower() for kw in ['sql', 'postgresql', 'mysql', 'database'])
            if has_backend and has_database and 5 <= n_skills <= 10:
                backend_db = c
        
        # Frontend + DevOps
        if not fullstack_devops:
            has_frontend = any(kw in str(skills).lower() for kw in ['react', 'vue', 'angular', 'frontend'])
            has_devops = any(kw in str(skills).lower() for kw in ['docker', 'kubernetes', 'ci/cd', 'cloud', 'aws'])
            if has_frontend and has_devops and n_skills >= 10:
                fullstack_devops = c
        
        # Mobile developer
        if not mobile_dev:
            has_mobile = any(kw in str(skills).lower() for kw in ['flutter', 'react native', 'ios', 'android', 'mobile'])
            if has_mobile:
                mobile_dev = c
        
        if backend_db and fullstack_devops and mobile_dev:
            break
    
    # Fallback to original logic if not enough diverse candidates
    if not backend_db:
        for c in candidates:
            if len(c['normalized_skills']) <= 5:
                backend_db = c
                break
    
    if not fullstack_devops:
        for c in candidates:
            if len(c['normalized_skills']) >= 15:
                fullstack_devops = c
                break
    
    if not mobile_dev:
        for c in candidates:
            if 8 <= len(c['normalized_skills']) <= 12:
                mobile_dev = c
                break
    
    # Extract features with both approaches
    old_engineer = FeatureEngineer()
    new_engineer = EnhancedFeatureEngineer(
        enable_inference=True,
        enable_reverse_inference=True,
        min_confidence=0.70,
        include_presence_features=True,
        include_domain_features=True
    )
    
    results = []
    for candidate in [backend_db, fullstack_devops, mobile_dev]:
        old_features = old_engineer.extract_features(
            candidate_id=candidate['candidate_id'],
            normalized_skills=candidate['normalized_skills'],
            normalized_scores=candidate['normalized_scores']
        )
        
        new_features = new_engineer.extract_features(
            candidate_id=candidate['candidate_id'],
            normalized_skills=candidate['normalized_skills'],
            normalized_scores=candidate['normalized_scores']
        )
        
        results.append({
            'candidate': candidate,
            'old_count': old_features['feature_count'],
            'new_count': new_features['feature_count'],
            'added': new_features['feature_count'] - old_features['feature_count'],
            'new_features': new_features['features']
        })
    
    # Create visualization
    fig, axes = plt.subplots(2, 2, figsize=(14, 10))
    fig.suptitle('Solution to "Too Many Zeros" Problem', fontsize=16, fontweight='bold')
    
    # Chart 1: Feature Count Comparison
    ax1 = axes[0, 0]
    x = np.arange(3)
    width = 0.35
    
    old_counts = [r['old_count'] for r in results]
    new_counts = [r['new_count'] for r in results]
    labels = [
        f"Backend+DB\n({len(backend_db['normalized_skills'])} skills)",
        f"Frontend+DevOps\n({len(fullstack_devops['normalized_skills'])} skills)",
        f"Mobile\n({len(mobile_dev['normalized_skills'])} skills)"
    ]
    
    bars1 = ax1.bar(x - width/2, old_counts, width, label='Old Approach', color='#e74c3c')
    bars2 = ax1.bar(x + width/2, new_counts, width, label='New Approach', color='#27ae60')
    
    ax1.set_ylabel('Feature Count', fontsize=12)
    ax1.set_title('Feature Count: Old vs New', fontsize=13, fontweight='bold')
    ax1.set_xticks(x)
    ax1.set_xticklabels(labels)
    ax1.legend()
    ax1.grid(axis='y', alpha=0.3)
    
    # Add value labels
    for bars in [bars1, bars2]:
        for bar in bars:
            height = bar.get_height()
            ax1.text(bar.get_x() + bar.get_width()/2., height,
                    f'{int(height)}',
                    ha='center', va='bottom', fontsize=10)
    
    # Chart 2: Added Features Breakdown
    ax2 = axes[0, 1]
    added_features = [r['added'] for r in results]
    colors_gradient = ['#3498db', '#9b59b6', '#f39c12']
    
    bars = ax2.bar(labels, added_features, color=colors_gradient, alpha=0.7)
    ax2.set_ylabel('Added Features', fontsize=12)
    ax2.set_title('Features Added by Enhancement', fontsize=13, fontweight='bold')
    ax2.grid(axis='y', alpha=0.3)
    
    for bar in bars:
        height = bar.get_height()
        ax2.text(bar.get_x() + bar.get_width()/2., height,
                f'+{int(height)}',
                ha='center', va='bottom', fontsize=11, fontweight='bold')
    
    # Chart 3: Feature Type Breakdown for Backend+DB
    ax3 = axes[1, 0]
    backend_db_features = results[0]['new_features']
    
    has_count = len([k for k in backend_db_features.keys() if k.startswith('has_')])
    domain_count = len([k for k in backend_db_features.keys() if k.startswith('domain_')])
    inference_count = len([k for k in backend_db_features.keys() if 'inference' in k])
    base_count = results[0]['old_count']
    
    sizes = [base_count, has_count, domain_count, inference_count]
    labels_pie = ['Base Features', 'Presence Flags', 'Domain Features', 'Inference Stats']
    colors_pie = ['#95a5a6', '#3498db', '#e67e22', '#9b59b6']
    explode = (0.05, 0.05, 0.05, 0.05)
    
    wedges, texts, autotexts = ax3.pie(sizes, explode=explode, labels=labels_pie,
                                        colors=colors_pie, autopct='%1.1f%%',
                                        shadow=True, startangle=90)
    
    for autotext in autotexts:
        autotext.set_color('white')
        autotext.set_fontweight('bold')
        autotext.set_fontsize(9)
    
    ax3.set_title('Feature Breakdown (Backend+DB)', fontsize=13, fontweight='bold')
    
    # Chart 4: Domain Presence Comparison
    ax4 = axes[1, 1]
    
    domains = ['frontend', 'backend', 'data', 'devops', 'mobile', 'database']
    
    backend_db_domain_presence = [
        results[0]['new_features'].get(f'domain_{d}_presence', 0) 
        for d in domains
    ]
    fullstack_domain_presence = [
        results[1]['new_features'].get(f'domain_{d}_presence', 0) 
        for d in domains
    ]
    mobile_domain_presence = [
        results[2]['new_features'].get(f'domain_{d}_presence', 0) 
        for d in domains
    ]
    
    x_domains = np.arange(len(domains))
    width_domain = 0.25
    
    bars1 = ax4.bar(x_domains - width_domain, backend_db_domain_presence, 
                    width_domain, label='Backend+DB', color='#e74c3c', alpha=0.7)
    bars2 = ax4.bar(x_domains, fullstack_domain_presence, 
                    width_domain, label='Frontend+DevOps', color='#27ae60', alpha=0.7)
    bars3 = ax4.bar(x_domains + width_domain, mobile_domain_presence, 
                    width_domain, label='Mobile', color='#3498db', alpha=0.7)
    
    ax4.set_ylabel('Domain Presence', fontsize=12)
    ax4.set_title('Domain Presence: 3 Different Profiles', fontsize=13, fontweight='bold')
    ax4.set_xticks(x_domains)
    ax4.set_xticklabels(domains, rotation=45, ha='right')
    ax4.legend()
    ax4.grid(axis='y', alpha=0.3)
    ax4.set_ylim(0, 1.0)
    
    plt.tight_layout()
    
    # Save figure
    output_path = Path(__file__).parent / 'zero_problem_solution_visualization.png'
    plt.savefig(output_path, dpi=300, bbox_inches='tight')
    print(f"\n✅ Visualization saved to: {output_path}")
    
    plt.show()
    
    # Print summary
    print("\n" + "="*80)
    print("SUMMARY")
    print("="*80)
    print(f"\nBackend+DB Specialist ({backend_db['candidate_id']}):")
    print(f"  Skills: {backend_db['normalized_skills'][:5]}...")
    print(f"  Total skills: {len(backend_db['normalized_skills'])}")
    print(f"  Old features: {results[0]['old_count']}")
    print(f"  New features: {results[0]['new_count']} (+{results[0]['added']})")
    
    print(f"\nFrontend+DevOps ({fullstack_devops['candidate_id']}):")
    print(f"  Skills: {fullstack_devops['normalized_skills'][:5]}...")
    print(f"  Total skills: {len(fullstack_devops['normalized_skills'])}")
    print(f"  Old features: {results[1]['old_count']}")
    print(f"  New features: {results[1]['new_count']} (+{results[1]['added']})")
    
    print(f"\nMobile Developer ({mobile_dev['candidate_id']}):")
    print(f"  Skills: {mobile_dev['normalized_skills'][:5]}...")
    print(f"  Total skills: {len(mobile_dev['normalized_skills'])}")
    print(f"  Old features: {results[2]['old_count']}")
    print(f"  New features: {results[2]['new_count']} (+{results[2]['added']})")
    
    # Show domain breakdown for each
    print("\n" + "="*80)
    print("DOMAIN PRESENCE BREAKDOWN")
    print("="*80)
    for i, (name, result) in enumerate([
        ("Backend+DB", results[0]),
        ("Frontend+DevOps", results[1]),
        ("Mobile", results[2])
    ]):
        print(f"\n{name}:")
        for domain in domains:
            presence = result['new_features'].get(f'domain_{domain}_presence', 0)
            strength = result['new_features'].get(f'domain_{domain}_strength', 0)
            if presence > 0:
                print(f"  {domain:12s}: presence={presence:.2f}, strength={strength:.2f}")


if __name__ == '__main__':
    visualize_zero_problem()
