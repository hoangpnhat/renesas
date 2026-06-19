"""
Apply Phase 3: Multi-Label Clustering v4 (DISTANCE-BASED)

Approach: Use distance to cluster centroids instead of GMM probabilities
- Calculate distance from each candidate to all cluster centroids
- Assign to multiple clusters if distance is within threshold
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
from sklearn.metrics import pairwise_distances
import umap
import hdbscan

print("="*70)
print("PHASE 3: MULTI-LABEL CLUSTERING (DISTANCE-BASED)")
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

# Clustering with MORE clusters
print("\n3. Running clustering...")

# GMM with more components
best_bic = float('inf')
best_k = None
best_gmm = None

for k in range(12, 18):  # Try 12-17 clusters
    gmm = GaussianMixture(n_components=k, random_state=42, n_init=3)
    gmm.fit(features_umap)
    bic = gmm.bic(features_umap)
    if bic < best_bic:
        best_bic = bic
        best_k = k
        best_gmm = gmm

print(f"   [OK] GMM: {best_k} clusters (best BIC)")

# Get primary cluster assignments
primary_labels = best_gmm.predict(features_umap)

# Get cluster centroids (means)
centroids = best_gmm.means_

# Multi-label assignment using DISTANCE to centroids
print("\n4. Computing multi-label cluster assignments (distance-based)...")

# Calculate distance from each candidate to all centroids
distances = pairwise_distances(features_umap, centroids, metric='euclidean')

# For each candidate, find clusters within distance threshold
candidate_clusters = []

for i in range(len(candidates)):
    candidate_distances = distances[i]
    primary_cluster = primary_labels[i]
    primary_distance = candidate_distances[primary_cluster]

    # Assign to clusters within threshold of primary distance
    # Threshold: within 150% of primary distance
    DISTANCE_MULTIPLIER = 1.5
    distance_threshold = primary_distance * DISTANCE_MULTIPLIER

    assigned_clusters = []
    for cluster_id in range(best_k):
        dist = candidate_distances[cluster_id]
        if dist <= distance_threshold:
            # Normalize distance to probability-like score (closer = higher score)
            normalized_score = 1 / (1 + dist)
            assigned_clusters.append({
                'cluster_id': cluster_id,
                'distance': float(dist),
                'score': float(normalized_score)
            })

    # Sort by score (closest first)
    assigned_clusters.sort(key=lambda x: x['score'], reverse=True)

    candidate_clusters.append({
        'candidate_id': candidates[i]['candidate_id'],
        'clusters': assigned_clusters,
        'primary_cluster': int(primary_cluster),  # Convert numpy int64 to Python int
        'num_clusters': len(assigned_clusters)
    })

n_clusters_final = best_k
print(f"   [OK] Final: {n_clusters_final} clusters")
print(f"   [OK] Multi-label assignments:")
multi_label_count = sum(1 for c in candidate_clusters if c['num_clusters'] > 1)
print(f"       - Candidates in 1 cluster: {sum(1 for c in candidate_clusters if c['num_clusters'] == 1)}")
print(f"       - Candidates in 2+ clusters: {multi_label_count}")
print(f"       - Avg clusters per candidate: {np.mean([c['num_clusters'] for c in candidate_clusters]):.2f}")
print(f"       - Max clusters per candidate: {max(c['num_clusters'] for c in candidate_clusters)}")

# Show distribution
cluster_count_dist = Counter([c['num_clusters'] for c in candidate_clusters])
for num_clusters in sorted(cluster_count_dist.keys()):
    count = cluster_count_dist[num_clusters]
    print(f"       - {count} candidates in {num_clusters} cluster(s)")

# Define skill categories for better labeling
print("\n5. Setting up skill categorization...")

GENERIC_SKILLS = {
    'backend', 'frontend', 'technical skills', 'backend development',
    'frontend development', 'full stack development', 'software development',
    'development', 'engineering', 'skills', 'technical', 'programming',
    'software engineering', 'application development', 'web development'
}

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

    # AI/ML & Data Science
    'machine learning', 'ai', 'deep learning', 'neural network',
    'tensorflow', 'pytorch', 'scikit-learn', 'pandas', 'numpy',
    'embedding generation', 'model evaluation', 'model training',
    'model deployment', 'mlops', 'langchain', 'openai', 'llm',
    'transformer models', 'large language models', 'model fine-tuning',
    'model optimization', 'model inference', 'convolutional neural network',
    'ai architecture', 'machine learning frameworks', 'faiss',

    # Other Specialized
    'graphql', 'rest api', 'grpc'
}

print(f"   - Excluding {len(GENERIC_SKILLS)} generic skills from labels")
print(f"   - Prioritizing {len(TECHNICAL_SKILLS)} specific technical skills")

# Generate cluster profiles
print("\n6. Generating cluster profiles...")

clusters_output = []

for cluster_id in range(n_clusters_final):
    # Get members (candidates with this cluster in their assignments)
    primary_members = [i for i, c in enumerate(candidate_clusters) if c['primary_cluster'] == cluster_id]
    all_members = [
        i for i, c in enumerate(candidate_clusters)
        if any(cl['cluster_id'] == cluster_id for cl in c['clusters'])
    ]

    member_ids = [candidates[i]['candidate_id'] for i in all_members]

    if not member_ids:
        continue

    print(f"\n   Cluster {cluster_id} ({len(member_ids)} total, {len(primary_members)} primary)...")

    # Aggregate skills and scores
    all_skills = []
    all_scores = defaultdict(list)

    for idx in all_members:
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

    # Identify STRENGTHS (lowered threshold to catch rare skills like AI/ML)
    strengths = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] >= 70 and stats['frequency_pct'] >= 0.10:  # Lowered from 0.20 to 0.10
            strengths.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Identify WEAKNESSES (lowered threshold to be consistent)
    weaknesses = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] < 65 and stats['frequency_pct'] >= 0.10:  # Lowered from 0.20 to 0.10
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
            if stats['avg_score'] < 65 and stats['frequency_pct'] >= 0.10:  # Lowered from 0.20 to 0.10
                if skill.lower() not in GENERIC_SKILLS:
                    weaknesses.append({
                        'skill': skill,
                        'avg_score': round(stats['avg_score'], 2),
                        'frequency_pct': round(stats['frequency_pct'] * 100, 1)
                    })

    # Generate cluster label
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
        'primary_members': len(primary_members),
        'secondary_members': len(all_members) - len(primary_members),
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
    print(f"     Weaknesses: {[w['skill'] for w in weaknesses[:2]]}")

# Save results
print("\n7. Saving multi-label clustering results...")

output = {
    'total_candidates': len(candidates),
    'total_clusters': len(clusters_output),
    'clustering_method': 'Multi-label GMM (distance-based)',
    'embedding_dims': features_umap.shape[1],
    'distance_multiplier': DISTANCE_MULTIPLIER,
    'multi_label_statistics': {
        'candidates_in_1_cluster': sum(1 for c in candidate_clusters if c['num_clusters'] == 1),
        'candidates_in_2plus_clusters': sum(1 for c in candidate_clusters if c['num_clusters'] > 1),
        'avg_clusters_per_candidate': float(np.mean([c['num_clusters'] for c in candidate_clusters])),
        'max_clusters_per_candidate': max([c['num_clusters'] for c in candidate_clusters]),
        'cluster_count_distribution': dict(cluster_count_dist)
    },
    'improvements': [
        'Increased clusters to 12-17 (optimal by BIC)',
        'Multi-label: Distance-based assignment',
        'Candidate assigned to clusters within 150% of primary distance',
        'Focus on specific technical skills'
    ],
    'clusters': clusters_output
}

with open('data/processed/clusters_final_multilabel.json', 'w', encoding='utf-8') as f:
    json.dump(output, f, indent=2, ensure_ascii=False)

print(f"   [OK] Saved to data/processed/clusters_final_multilabel.json")

# Save multi-label cluster assignments
with open('data/processed/cluster_assignments_multilabel.json', 'w', encoding='utf-8') as f:
    json.dump(candidate_clusters, f, indent=2)

print(f"   [OK] Saved multi-label cluster assignments")

# Summary
print("\n" + "="*70)
print("PHASE 3 COMPLETE - MULTI-LABEL CLUSTERING (DISTANCE-BASED)")
print("="*70)

print(f"\nClustering Results:")
print(f"  - Total candidates: {len(candidates)}")
print(f"  - Total clusters: {len(clusters_output)}")
print(f"  - Candidates in 1 cluster: {output['multi_label_statistics']['candidates_in_1_cluster']}")
print(f"  - Candidates in 2+ clusters: {output['multi_label_statistics']['candidates_in_2plus_clusters']}")
print(f"  - Avg clusters per candidate: {output['multi_label_statistics']['avg_clusters_per_candidate']:.2f}")
print(f"  - Max clusters per candidate: {output['multi_label_statistics']['max_clusters_per_candidate']}")

print(f"\nCluster Distribution:")
for cluster in clusters_output:
    size_info = f"{cluster['size']:3} members ({cluster['primary_members']} primary, {cluster['secondary_members']} secondary)"
    weaknesses_str = f" | Weaknesses: {len(cluster['profile']['weaknesses'])}" if cluster['profile']['weaknesses'] else ""
    print(f"  {cluster['cluster_id']:15} | {size_info:45} | {cluster['cluster_label']}{weaknesses_str}")

print("\n" + "="*70)
print("READY FOR MULTI-LABEL INFERENCE!")
print("="*70)
