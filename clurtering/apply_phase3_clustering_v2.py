"""
Apply Phase 3: Clustering with Multi-Label Generation (IMPROVED)

Improvements:
1. Focus on specific technical skills (languages, frameworks) for labels
2. Adjusted weakness detection threshold
3. Better label generation
"""

import sys
from pathlib import Path
import json
import numpy as np
import pandas as pd
from collections import Counter, defaultdict
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler
from sklearn.mixture import GaussianMixture
import umap
import hdbscan

print("="*70)
print("PHASE 3: CLUSTERING & MULTI-LABEL GENERATION (IMPROVED)")
print("="*70)

# Load Phase 2 data
print("\n1. Loading Phase 2 feature data...")
feature_matrix = np.load('data/processed/feature_matrix.npy')
print(f"   - Feature matrix shape: {feature_matrix.shape}")

with open('data/processed/feature_names.json', 'r') as f:
    feature_names = json.load(f)

with open('data/processed/candidates_features.json', 'r', encoding='utf-8') as f:
    candidates_data = json.load(f)
candidates = candidates_data['candidates']

with open('data/processed/candidates_normalized_tier2.json', 'r', encoding='utf-8') as f:
    candidates_normalized = json.load(f)

# Dimensionality Reduction (reuse from previous run)
print("\n2. Loading embeddings...")
features_umap = np.load('data/processed/embeddings_25d.npy')
print(f"   [OK] Loaded embeddings shape: {features_umap.shape}")

# Clustering (reuse from previous run)
print("\n3. Re-running clustering...")

hdbscan_clusterer = hdbscan.HDBSCAN(
    min_cluster_size=5,
    min_samples=3,
    cluster_selection_epsilon=0.0,
    metric='euclidean'
)
hdbscan_labels = hdbscan_clusterer.fit_predict(features_umap)
n_clusters_hdbscan = len(set(hdbscan_labels)) - (1 if -1 in hdbscan_labels else 0)
n_noise = sum(hdbscan_labels == -1)
print(f"   [OK] HDBSCAN: {n_clusters_hdbscan} clusters, {n_noise} noise points")

# GMM
best_bic = float('inf')
best_k = None
best_gmm = None

for k in range(5, 15):
    gmm = GaussianMixture(n_components=k, random_state=42, n_init=3)
    gmm.fit(features_umap)
    bic = gmm.bic(features_umap)
    if bic < best_bic:
        best_bic = bic
        best_k = k
        best_gmm = gmm

gmm_labels = best_gmm.predict(features_umap)
print(f"   [OK] GMM: {best_k} clusters")

# Ensemble
final_labels = hdbscan_labels.copy()
for i in range(len(final_labels)):
    if final_labels[i] == -1:
        final_labels[i] = gmm_labels[i] + n_clusters_hdbscan

n_clusters_final = len(set(final_labels))
print(f"   [OK] Final: {n_clusters_final} clusters")

# Define skill categories for better labeling
print("\n4. Setting up skill categorization...")

# Generic skills to EXCLUDE from labels (too broad)
GENERIC_SKILLS = {
    'backend', 'frontend', 'technical skills', 'backend development',
    'frontend development', 'full stack development', 'software development',
    'development', 'engineering', 'skills', 'technical', 'programming',
    'software engineering', 'application development', 'web development'
}

# Specific technical skills to PRIORITIZE for labels
TECHNICAL_SKILLS = {
    # Programming Languages
    'python', 'java', 'javascript', 'typescript', 'go', 'golang',
    'ruby', 'php', 'c#', 'csharp', 'scala', 'kotlin', 'rust', 'c++',

    # Frameworks
    'react', 'vue', 'angular', 'next.js', 'nextjs', 'svelte',
    'django', 'flask', 'fastapi', 'spring', 'spring boot',
    'express', 'express.js', 'nodejs', 'node.js', '.net',

    # Databases
    'postgresql', 'mysql', 'mongodb', 'redis', 'elasticsearch',
    'cassandra', 'dynamodb', 'sql', 'nosql',

    # DevOps & Cloud
    'docker', 'kubernetes', 'k8s', 'jenkins', 'terraform',
    'ansible', 'aws', 'azure', 'gcp', 'kafka',

    # Specialized
    'tensorflow', 'pytorch', 'pandas', 'numpy', 'scikit-learn',
    'graphql', 'rest api', 'grpc'
}

print(f"   - Excluding {len(GENERIC_SKILLS)} generic skills from labels")
print(f"   - Prioritizing {len(TECHNICAL_SKILLS)} specific technical skills")

# Generate Multi-Label Cluster Profiles (IMPROVED)
print("\n5. Generating improved multi-label cluster profiles...")

clusters_output = []

for cluster_id in sorted(set(final_labels)):
    member_indices = np.where(final_labels == cluster_id)[0]
    member_ids = [candidates[i]['candidate_id'] for i in member_indices]

    print(f"\n   Cluster {cluster_id} ({len(member_ids)} members)...")

    # Aggregate skills and scores
    all_skills = []
    all_scores = defaultdict(list)

    for idx in member_indices:
        candidate = candidates_normalized[idx]
        all_skills.extend(candidate['normalized_skills'])

        for skill, score in candidate['normalized_scores'].items():
            all_scores[skill].append(score)

    # Calculate skill statistics
    skill_stats = {}
    for skill, scores in all_scores.items():
        skill_stats[skill] = {
            'avg_score': np.mean(scores),
            'frequency': len(scores),
            'frequency_pct': len(scores) / len(member_ids)
        }

    # Filter to technical skills only
    technical_skill_stats = {
        skill: stats for skill, stats in skill_stats.items()
        if skill.lower() in TECHNICAL_SKILLS
    }

    # Sort by importance (frequency * score)
    sorted_technical = sorted(
        technical_skill_stats.items(),
        key=lambda x: x[1]['frequency_pct'] * x[1]['avg_score'],
        reverse=True
    )

    # Identify STRENGTHS (specific technical skills with high scores)
    # RELAXED threshold: avg_score >= 70 AND present in >= 25% of members
    strengths = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] >= 70 and stats['frequency_pct'] >= 0.25:
            strengths.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Identify WEAKNESSES (IMPROVED detection)
    # NEW threshold: avg_score < 65 AND present in >= 20% of members
    weaknesses = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] < 65 and stats['frequency_pct'] >= 0.20:
            weaknesses.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # If no technical weaknesses, check all skills
    if not weaknesses:
        all_sorted = sorted(
            skill_stats.items(),
            key=lambda x: x[1]['avg_score']
        )
        for skill, stats in all_sorted:
            if stats['avg_score'] < 65 and stats['frequency_pct'] >= 0.20:
                if skill.lower() not in GENERIC_SKILLS:
                    weaknesses.append({
                        'skill': skill,
                        'avg_score': round(stats['avg_score'], 2),
                        'frequency_pct': round(stats['frequency_pct'] * 100, 1)
                    })

    # Generate IMPROVED cluster label
    label_parts = []

    # Add top 3 SPECIFIC technical strengths
    for strength in strengths[:3]:
        skill_name = strength['skill'].replace(' ', '').replace('.', '').replace('-', '').title()
        label_parts.append(f"Strong{skill_name}")

    # Add top 2 weaknesses
    for weakness in weaknesses[:2]:
        skill_name = weakness['skill'].replace(' ', '').replace('.', '').replace('-', '').title()
        label_parts.append(f"Weak{skill_name}")

    # If no specific technical skills found, use top generic skills
    if not label_parts:
        top_general = sorted(
            [(s, st) for s, st in skill_stats.items() if s.lower() not in GENERIC_SKILLS],
            key=lambda x: x[1]['frequency_pct'] * x[1]['avg_score'],
            reverse=True
        )[:3]

        for skill, stats in top_general:
            skill_name = skill.replace(' ', '').title()
            label_parts.append(f"Strong{skill_name}")

    cluster_label = "_".join(label_parts) if label_parts else f"MixedProfile{cluster_id}"

    # Calculate domain scores
    domain_scores = {}
    domain_mapping = {
        'python': 'Backend', 'java': 'Backend', 'go': 'Backend', 'golang': 'Backend',
        'javascript': 'Frontend', 'typescript': 'Frontend', 'react': 'Frontend',
        'vue': 'Frontend', 'angular': 'Frontend',
        'postgresql': 'Database', 'mysql': 'Database', 'mongodb': 'Database',
        'docker': 'DevOps', 'kubernetes': 'DevOps', 'k8s': 'DevOps',
        'aws': 'Cloud', 'azure': 'Cloud', 'gcp': 'Cloud'
    }

    domain_skill_scores = defaultdict(list)
    for skill, stats in skill_stats.items():
        if skill.lower() in domain_mapping:
            domain = domain_mapping[skill.lower()]
            domain_skill_scores[domain].append(stats['avg_score'])

    for domain, scores in domain_skill_scores.items():
        if scores:
            domain_scores[domain] = round(np.mean(scores), 2)

    # Profile type
    num_domains = len(domain_scores)
    max_domain_score = max(domain_scores.values()) if domain_scores else 0
    avg_domain_score = np.mean(list(domain_scores.values())) if domain_scores else 0

    if num_domains <= 2 and max_domain_score >= 80:
        profile_type = "Specialist"
    elif num_domains >= 4 and avg_domain_score >= 70:
        profile_type = "Generalist"
    else:
        profile_type = "Balanced"

    # Generate description
    strength_skills = [s['skill'] for s in strengths[:3]]
    weakness_skills = [w['skill'] for w in weaknesses[:2]]

    description_parts = []
    if strength_skills:
        description_parts.append(f"Strong in {', '.join(strength_skills)}")
    if weakness_skills:
        description_parts.append(f"weak in {', '.join(weakness_skills)}")

    cluster_description = "; ".join(description_parts) if description_parts else "Mixed skill profile"

    # Create cluster object
    cluster_obj = {
        'cluster_id': f"cluster_{cluster_id}",
        'cluster_label': cluster_label,
        'cluster_description': cluster_description,
        'size': len(member_ids),
        'profile': {
            'strengths': strengths[:5],
            'weaknesses': weaknesses[:5],
            'domain_scores': domain_scores,
            'profile_type': profile_type,
            'breadth': num_domains,
            'specialization_score': round(max_domain_score - avg_domain_score, 2) if domain_scores else 0
        },
        'members': member_ids
    }

    clusters_output.append(cluster_obj)

    print(f"     Label: {cluster_label}")
    print(f"     Profile: {profile_type}")
    print(f"     Strengths: {[s['skill'] for s in strengths[:3]]}")
    print(f"     Weaknesses: {[w['skill'] for w in weaknesses[:3]]}")

# Save results
print("\n6. Saving improved clustering results...")

output = {
    'total_candidates': len(candidates),
    'total_clusters': n_clusters_final,
    'clustering_method': 'Ensemble (HDBSCAN + GMM) - Improved labeling',
    'embedding_dims': features_umap.shape[1],
    'improvements': [
        'Focus on specific technical skills (languages, frameworks)',
        'Exclude generic skills from labels',
        'Relaxed weakness detection (< 65 score, >= 20% frequency)',
        'Better label generation'
    ],
    'clusters': clusters_output
}

with open('data/processed/clusters_final.json', 'w', encoding='utf-8') as f:
    json.dump(output, f, indent=2, ensure_ascii=False)

print(f"   [OK] Saved to data/processed/clusters_final.json")

# Save cluster assignments
cluster_assignments = [
    {
        'candidate_id': candidates[i]['candidate_id'],
        'cluster_id': f"cluster_{final_labels[i]}",
        'cluster_label': next(c['cluster_label'] for c in clusters_output if c['cluster_id'] == f"cluster_{final_labels[i]}")
    }
    for i in range(len(candidates))
]

with open('data/processed/cluster_assignments.json', 'w', encoding='utf-8') as f:
    json.dump(cluster_assignments, f, indent=2)

print(f"   [OK] Saved cluster assignments")

# Summary
print("\n" + "="*70)
print("PHASE 3 COMPLETE - IMPROVED CLUSTERING")
print("="*70)

print(f"\nClustering Results:")
print(f"  - Total candidates: {len(candidates)}")
print(f"  - Total clusters: {n_clusters_final}")

print(f"\nCluster Distribution (IMPROVED LABELS):")
for cluster in clusters_output:
    weaknesses_str = f" | Weaknesses: {len(cluster['profile']['weaknesses'])}" if cluster['profile']['weaknesses'] else ""
    print(f"  {cluster['cluster_id']:12} | {cluster['size']:3} members | {cluster['cluster_label']}{weaknesses_str}")

print("\n" + "="*70)
print("READY FOR INFERENCE!")
print("="*70)
