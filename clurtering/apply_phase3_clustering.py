"""
Apply Phase 3: Clustering with Multi-Label Generation

Steps:
1. Dimensionality reduction: 1,902 dims -> 20-30 dims (PCA + UMAP)
2. Ensemble clustering: HDBSCAN + GMM
3. Generate multi-label cluster profiles
4. Create labels like "StrongPython_StrongDocker_WeakKubernetes"
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
print("PHASE 3: CLUSTERING & MULTI-LABEL GENERATION")
print("="*70)

# Load Phase 2 data
print("\n1. Loading Phase 2 feature data...")
feature_matrix = np.load('data/processed/feature_matrix.npy')
print(f"   - Feature matrix shape: {feature_matrix.shape}")

with open('data/processed/feature_names.json', 'r') as f:
    feature_names = json.load(f)
print(f"   - Feature names: {len(feature_names)}")

with open('data/processed/candidates_features.json', 'r', encoding='utf-8') as f:
    candidates_data = json.load(f)
candidates = candidates_data['candidates']
print(f"   - Candidates: {len(candidates)}")

# Load original normalized data for skills
with open('data/processed/candidates_normalized_tier2.json', 'r', encoding='utf-8') as f:
    candidates_normalized = json.load(f)

# Dimensionality Reduction
print("\n2. Dimensionality Reduction (1,902 dims -> 20-30 dims)...")

# Step 1: Standardize features
print("   Step 1: Standardizing features...")
scaler = StandardScaler()
features_scaled = scaler.fit_transform(feature_matrix)
print(f"   [OK] Scaled to mean=0, std=1")

# Step 2: PCA (1,902 -> 100 dims)
print("   Step 2: PCA (1,902 -> 100 dims)...")
pca = PCA(n_components=100, random_state=42)
features_pca = pca.fit_transform(features_scaled)
explained_var = pca.explained_variance_ratio_.sum()
print(f"   [OK] Reduced to 100 dims (explained variance: {explained_var:.2%})")

# Step 3: UMAP (100 -> 25 dims)
print("   Step 3: UMAP (100 -> 25 dims)...")
print("   (This may take 2-3 minutes...)")
umap_reducer = umap.UMAP(
    n_components=25,
    n_neighbors=15,
    min_dist=0.1,
    metric='euclidean',
    random_state=42
)
features_umap = umap_reducer.fit_transform(features_pca)
print(f"   [OK] Final embedding shape: {features_umap.shape}")

# Save embeddings
np.save('data/processed/embeddings_25d.npy', features_umap)
print(f"   [OK] Saved embeddings to data/processed/embeddings_25d.npy")

# Clustering
print("\n3. Ensemble Clustering...")

# Method 1: HDBSCAN
print("   Method 1: HDBSCAN...")
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

# Method 2: GMM (Gaussian Mixture Model)
print("   Method 2: GMM...")
# Try different k values and pick best by BIC
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
print(f"   [OK] GMM: {best_k} clusters (best BIC)")

# Ensemble: Combine HDBSCAN + GMM
print("   Method 3: Ensemble (combining HDBSCAN + GMM)...")
# Use HDBSCAN as primary, GMM for noise points
final_labels = hdbscan_labels.copy()
for i in range(len(final_labels)):
    if final_labels[i] == -1:  # Noise point
        # Assign to GMM cluster
        final_labels[i] = gmm_labels[i] + n_clusters_hdbscan  # Offset to avoid conflict

n_clusters_final = len(set(final_labels))
print(f"   [OK] Final: {n_clusters_final} clusters")

# Analyze clusters
print("\n4. Analyzing cluster quality...")
cluster_sizes = Counter(final_labels)
print(f"   Cluster size distribution:")
for cluster_id in sorted(cluster_sizes.keys()):
    print(f"     Cluster {cluster_id}: {cluster_sizes[cluster_id]} candidates")

# Generate Multi-Label Cluster Profiles
print("\n5. Generating multi-label cluster profiles...")

clusters_output = []

for cluster_id in sorted(set(final_labels)):
    # Get members
    member_indices = np.where(final_labels == cluster_id)[0]
    member_ids = [candidates[i]['candidate_id'] for i in member_indices]

    print(f"\n   Processing Cluster {cluster_id} ({len(member_ids)} members)...")

    # Aggregate skills and scores across cluster members
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

    # Sort by frequency * avg_score (importance)
    sorted_skills = sorted(
        skill_stats.items(),
        key=lambda x: x[1]['frequency_pct'] * x[1]['avg_score'],
        reverse=True
    )

    # Identify strengths (top skills with high scores)
    strengths = []
    for skill, stats in sorted_skills[:10]:
        if stats['avg_score'] >= 75 and stats['frequency_pct'] >= 0.3:  # In >30% of members
            strengths.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Identify weaknesses (low scores, present in cluster)
    weaknesses = []
    for skill, stats in sorted_skills:
        if stats['avg_score'] < 60 and stats['frequency_pct'] >= 0.3:
            weaknesses.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Generate cluster label (multi-label format)
    label_parts = []

    # Add top 3 strengths
    for strength in strengths[:3]:
        skill_name = strength['skill'].replace(' ', '').title()
        label_parts.append(f"Strong{skill_name}")

    # Add top 2 weaknesses
    for weakness in weaknesses[:2]:
        skill_name = weakness['skill'].replace(' ', '').title()
        label_parts.append(f"Weak{skill_name}")

    cluster_label = "_".join(label_parts) if label_parts else f"Cluster{cluster_id}"

    # Calculate domain scores
    domain_scores = {}
    domain_mapping = {
        'backend': ['Backend'],
        'frontend': ['Frontend'],
        'database': ['Database'],
        'devops': ['DevOps'],
        'cloud': ['Cloud'],
        'data': ['Data/ML'],
        'api': ['Backend'],
        'microservices': ['Backend', 'Architecture'],
        'testing': ['Testing']
    }

    domain_skill_scores = defaultdict(list)
    for skill, stats in skill_stats.items():
        # Simple domain detection
        skill_lower = skill.lower()
        for keyword, domains in domain_mapping.items():
            if keyword in skill_lower:
                for domain in domains:
                    domain_skill_scores[domain].append(stats['avg_score'])

    for domain, scores in domain_skill_scores.items():
        if scores:
            domain_scores[domain] = round(np.mean(scores), 2)

    # Determine profile type
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
            'strengths': strengths[:5],  # Top 5
            'weaknesses': weaknesses[:5],  # Top 5
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
print("\n6. Saving clustering results...")

output = {
    'total_candidates': len(candidates),
    'total_clusters': n_clusters_final,
    'clustering_method': 'Ensemble (HDBSCAN + GMM)',
    'embedding_dims': features_umap.shape[1],
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

print(f"   [OK] Saved cluster assignments to data/processed/cluster_assignments.json")

# Create summary
print("\n" + "="*70)
print("PHASE 3 COMPLETE - CLUSTERING SUMMARY")
print("="*70)

print(f"\nClustering Results:")
print(f"  - Total candidates: {len(candidates)}")
print(f"  - Total clusters: {n_clusters_final}")
print(f"  - Embedding dimensions: {features_umap.shape[1]}")

print(f"\nCluster Distribution:")
for cluster in clusters_output:
    print(f"  {cluster['cluster_id']:12} | {cluster['size']:3} members | {cluster['cluster_label']}")

print(f"\nFiles Created:")
print(f"  1. data/processed/embeddings_25d.npy - 25D embeddings")
print(f"  2. data/processed/clusters_final.json - Cluster profiles with labels")
print(f"  3. data/processed/cluster_assignments.json - Candidate assignments")

print("\n" + "="*70)
print("READY FOR INFERENCE!")
print("="*70)
print("\nYou can now:")
print("  1. Query by cluster label")
print("  2. Find candidates matching skill requirements")
print("  3. Build recommendation system")
print("="*70)
