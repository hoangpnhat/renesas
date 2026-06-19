"""
Apply Phase 3: Clustering v5 (SKILL-WEIGHTED + FILTERING)

IMPROVEMENTS:
- Solution 1: Skill-Weighted Assignment (50% distance + 50% skill overlap)
- Solution 2: Post-Assignment Filtering (require >= 2/5 cluster skills match)

This addresses the misalignment issue where candidates were assigned to clusters
without having the main skills of that cluster.
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

print("="*80)
print("PHASE 3 v5: SKILL-WEIGHTED CLUSTERING + POST-FILTERING")
print("="*80)
print("\nIMPROVEMENTS:")
print("  - Solution 1: Combine distance (50%) + skill overlap (50%)")
print("  - Solution 2: Filter candidates not matching cluster skills")
print("="*80)

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

# Create mapping for quick lookup
candidate_id_to_idx = {c['candidate_id']: i for i, c in enumerate(candidates_normalized)}

print(f"   - Candidates: {len(candidates_normalized)}")

# Dimensionality Reduction (reuse from previous run)
print("\n2. Loading embeddings...")
features_umap = np.load('data/processed/embeddings_25d.npy')
print(f"   [OK] Loaded embeddings shape: {features_umap.shape}")

# Clustering
print("\n3. Running clustering...")

# GMM with optimal number of clusters
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

# Calculate distance from each candidate to all centroids
distances = pairwise_distances(features_umap, centroids, metric='euclidean')

# Define skill categories for better labeling
print("\n4. Setting up skill categorization...")

GENERIC_SKILLS = {
    'backend', 'frontend', 'technical skills', 'backend development',
    'frontend development', 'full stack development', 'software development',
    'development', 'engineering', 'skills', 'technical', 'programming',
    'software engineering', 'application development', 'web development',
    'full-stack development'
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
    'microservices', 'rest api', 'graphql', 'grpc',

    # AI/ML & Data Science
    'machine learning', 'ai', 'deep learning', 'neural network',
    'tensorflow', 'pytorch', 'scikit-learn', 'pandas', 'numpy',
    'embedding generation', 'model evaluation', 'model training',
    'model deployment', 'mlops', 'langchain', 'openai', 'llm',
    'transformer models', 'large language models', 'model fine-tuning',
    'model optimization', 'model inference', 'convolutional neural network',
    'ai architecture', 'machine learning frameworks', 'faiss',
}

print(f"   - Excluding {len(GENERIC_SKILLS)} generic skills from labels")
print(f"   - Prioritizing {len(TECHNICAL_SKILLS)} specific technical skills")

# First pass: Generate preliminary cluster profiles
print("\n5. Generating preliminary cluster profiles...")

preliminary_clusters = []

for cluster_id in range(best_k):
    # Get primary members only for initial profiling
    primary_members_idx = [i for i, label in enumerate(primary_labels) if label == cluster_id]

    if not primary_members_idx:
        continue

    # Aggregate skills and scores
    all_scores = defaultdict(list)

    for idx in primary_members_idx:
        candidate = candidates_normalized[idx]
        for skill, score in candidate['normalized_scores'].items():
            all_scores[skill].append(score)

    # Calculate skill statistics
    skill_stats = {}
    for skill, scores in all_scores.items():
        skill_stats[skill] = {
            'avg_score': np.mean(scores),
            'frequency': len(scores),
            'frequency_pct': len(scores) / len(primary_members_idx)
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

    # Identify STRENGTHS
    strengths = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] >= 70 and stats['frequency_pct'] >= 0.10:
            strengths.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Identify WEAKNESSES
    weaknesses = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] < 65 and stats['frequency_pct'] >= 0.10:
            weaknesses.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    preliminary_clusters.append({
        'cluster_id': cluster_id,
        'centroid': centroids[cluster_id],
        'strengths': strengths,
        'weaknesses': weaknesses,
        'primary_members_idx': primary_members_idx
    })

    print(f"   Cluster {cluster_id}: {len(strengths)} strengths, {len(weaknesses)} weaknesses")

# =============================
# SOLUTION 1: SKILL-WEIGHTED ASSIGNMENT
# =============================

def compute_skill_overlap_score(candidate, cluster_strengths, top_k=5, min_score_threshold=65):
    """
    Compute skill overlap score between candidate and cluster strengths.

    Args:
        candidate: Candidate dict with 'normalized_skills' and 'normalized_scores'
        cluster_strengths: List of cluster strength dicts with 'skill' and 'avg_score'
        top_k: Number of top cluster strengths to consider
        min_score_threshold: Minimum score for candidate to be considered having the skill

    Returns:
        float: Overlap score between 0 and 1
    """
    if not cluster_strengths:
        return 0.0

    top_cluster_skills = [s['skill'] for s in cluster_strengths[:top_k]]

    matches = 0
    weighted_matches = 0.0

    for cluster_skill in top_cluster_skills:
        candidate_score = candidate['normalized_scores'].get(cluster_skill, 0)

        if candidate_score >= min_score_threshold:
            matches += 1
            # Weight by how good the candidate is at this skill
            weighted_matches += candidate_score / 100.0

    # Normalize
    overlap_score = weighted_matches / len(top_cluster_skills) if top_cluster_skills else 0
    return overlap_score


def assign_clusters_with_skill_weighting(
    candidate_idx,
    candidate,
    clusters,
    distances_row,
    distance_weight=0.5,
    skill_weight=0.5,
    distance_multiplier=1.5
):
    """
    Assign candidate to clusters using both distance and skill overlap.

    Args:
        candidate_idx: Index of candidate
        candidate: Candidate dict
        clusters: List of preliminary cluster dicts
        distances_row: Distance from candidate to all centroids
        distance_weight: Weight for distance score (default 0.5)
        skill_weight: Weight for skill overlap score (default 0.5)
        distance_multiplier: Threshold multiplier for maximum distance

    Returns:
        List of cluster assignments with scores
    """
    # Find primary cluster (closest by distance)
    primary_cluster_id = np.argmin(distances_row)
    primary_distance = distances_row[primary_cluster_id]
    distance_threshold = primary_distance * distance_multiplier

    assignments = []

    for cluster in clusters:
        cluster_id = cluster['cluster_id']
        dist = distances_row[cluster_id]

        # Skip if too far
        if dist > distance_threshold:
            continue

        # 1. Distance score (closer = higher score)
        distance_score = 1 / (1 + dist)

        # 2. Skill overlap score
        skill_score = compute_skill_overlap_score(
            candidate,
            cluster['strengths'],
            top_k=5,
            min_score_threshold=65
        )

        # 3. Combined score
        final_score = (distance_weight * distance_score) + (skill_weight * skill_score)

        assignments.append({
            'cluster_id': cluster_id,
            'distance': float(dist),
            'distance_score': float(distance_score),
            'skill_score': float(skill_score),
            'final_score': float(final_score)
        })

    # Sort by final score
    assignments.sort(key=lambda x: x['final_score'], reverse=True)

    return assignments


print("\n6. Skill-weighted cluster assignment...")
print("   Using 50% distance + 50% skill overlap")

candidate_clusters = []
assignment_stats = {
    'total_assignments': 0,
    'avg_skill_score': [],
    'avg_distance_score': [],
    'avg_final_score': []
}

for i in range(len(candidates_normalized)):
    candidate = candidates_normalized[i]

    assignments = assign_clusters_with_skill_weighting(
        candidate_idx=i,
        candidate=candidate,
        clusters=preliminary_clusters,
        distances_row=distances[i],
        distance_weight=0.5,
        skill_weight=0.5,
        distance_multiplier=1.5
    )

    if assignments:
        assignment_stats['total_assignments'] += len(assignments)
        assignment_stats['avg_skill_score'].extend([a['skill_score'] for a in assignments])
        assignment_stats['avg_distance_score'].extend([a['distance_score'] for a in assignments])
        assignment_stats['avg_final_score'].extend([a['final_score'] for a in assignments])

    candidate_clusters.append({
        'candidate_id': candidates[i]['candidate_id'],
        'clusters': assignments,
        'primary_cluster': assignments[0]['cluster_id'] if assignments else -1,
        'num_clusters': len(assignments)
    })

print(f"   [OK] Initial assignments complete")
print(f"   - Avg skill overlap score: {np.mean(assignment_stats['avg_skill_score']):.3f}")
print(f"   - Avg distance score: {np.mean(assignment_stats['avg_distance_score']):.3f}")
print(f"   - Avg final score: {np.mean(assignment_stats['avg_final_score']):.3f}")

# =============================
# SOLUTION 2: POST-ASSIGNMENT FILTERING
# =============================

def check_cluster_skill_match(candidate, cluster, min_matches=2, top_k=5, min_score_threshold=65):
    """
    Check if candidate has at least min_matches of cluster's top skills.

    Args:
        candidate: Candidate dict
        cluster: Cluster dict with 'strengths'
        min_matches: Minimum number of matching skills required
        top_k: Number of top cluster skills to check
        min_score_threshold: Minimum score for match

    Returns:
        tuple: (passes_filter, num_matches, total_checked)
    """
    cluster_top_skills = [s['skill'] for s in cluster['strengths'][:top_k]]

    if not cluster_top_skills:
        # If cluster has no defined strengths, allow assignment
        return True, 0, 0

    matches = 0
    for skill in cluster_top_skills:
        candidate_score = candidate['normalized_scores'].get(skill, 0)
        if candidate_score >= min_score_threshold:
            matches += 1

    passes = matches >= min_matches
    return passes, matches, len(cluster_top_skills)


print("\n7. Post-assignment filtering...")
print("   Filtering candidates without cluster skill matches")

filtered_candidates = []
filter_stats = {
    'reassigned': 0,
    'became_outliers': 0,
    'passed_all': 0,
    'failed_checks': []
}

for assignment in candidate_clusters:
    candidate_id = assignment['candidate_id']
    candidate_idx = candidate_id_to_idx[candidate_id]
    candidate = candidates_normalized[candidate_idx]

    filtered_clusters = []

    for cluster_assignment in assignment['clusters']:
        cluster_id = cluster_assignment['cluster_id']
        cluster = preliminary_clusters[cluster_id]

        # Check if candidate matches cluster skills
        passes, num_matches, total_checked = check_cluster_skill_match(
            candidate,
            cluster,
            min_matches=2,
            top_k=5,
            min_score_threshold=65
        )

        if passes:
            filtered_clusters.append(cluster_assignment)
        else:
            filter_stats['failed_checks'].append({
                'candidate_id': candidate_id,
                'cluster_id': cluster_id,
                'matches': num_matches,
                'total': total_checked
            })

    # Update assignment
    if len(filtered_clusters) < len(assignment['clusters']):
        filter_stats['reassigned'] += 1

    if not filtered_clusters:
        # No clusters passed - mark as outlier
        filter_stats['became_outliers'] += 1
        filtered_candidates.append({
            'candidate_id': candidate_id,
            'clusters': [],
            'primary_cluster': -1,  # -1 indicates outlier
            'num_clusters': 0,
            'outlier_reason': 'No cluster skill match'
        })
    else:
        if len(filtered_clusters) == len(assignment['clusters']):
            filter_stats['passed_all'] += 1

        filtered_candidates.append({
            'candidate_id': candidate_id,
            'clusters': filtered_clusters,
            'primary_cluster': filtered_clusters[0]['cluster_id'],
            'num_clusters': len(filtered_clusters)
        })

print(f"   [OK] Filtering complete")
print(f"   - Candidates passed all checks: {filter_stats['passed_all']}")
print(f"   - Candidates reassigned: {filter_stats['reassigned']}")
print(f"   - Candidates became outliers: {filter_stats['became_outliers']}")
print(f"   - Total failed checks: {len(filter_stats['failed_checks'])}")

# Update candidate_clusters with filtered results
candidate_clusters = filtered_candidates

# Compute multi-label statistics
multi_label_stats = {
    'candidates_in_0_clusters': sum(1 for c in candidate_clusters if c['num_clusters'] == 0),
    'candidates_in_1_cluster': sum(1 for c in candidate_clusters if c['num_clusters'] == 1),
    'candidates_in_2plus_clusters': sum(1 for c in candidate_clusters if c['num_clusters'] > 1),
    'avg_clusters_per_candidate': float(np.mean([c['num_clusters'] for c in candidate_clusters])),
    'max_clusters_per_candidate': max([c['num_clusters'] for c in candidate_clusters])
}

print(f"\n   Multi-label statistics (after filtering):")
print(f"   - Outliers (0 clusters): {multi_label_stats['candidates_in_0_clusters']}")
print(f"   - Candidates in 1 cluster: {multi_label_stats['candidates_in_1_cluster']}")
print(f"   - Candidates in 2+ clusters: {multi_label_stats['candidates_in_2plus_clusters']}")
print(f"   - Avg clusters per candidate: {multi_label_stats['avg_clusters_per_candidate']:.2f}")

# Generate final cluster profiles
print("\n8. Generating final cluster profiles...")

clusters_output = []

for cluster_id in range(best_k):
    # Get members after filtering
    all_members_idx = [
        candidate_id_to_idx[c['candidate_id']]
        for c in candidate_clusters
        if any(cl['cluster_id'] == cluster_id for cl in c['clusters'])
    ]

    primary_members_idx = [
        candidate_id_to_idx[c['candidate_id']]
        for c in candidate_clusters
        if c['primary_cluster'] == cluster_id
    ]

    member_ids = [candidates_normalized[i]['candidate_id'] for i in all_members_idx]

    if not member_ids:
        continue

    print(f"\n   Cluster {cluster_id} ({len(member_ids)} total, {len(primary_members_idx)} primary)...")

    # Aggregate skills and scores from ALL members
    all_scores = defaultdict(list)

    for idx in all_members_idx:
        candidate = candidates_normalized[idx]
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

    # Filter to technical skills
    technical_skill_stats = {
        skill: stats for skill, stats in skill_stats.items()
        if skill.lower() in TECHNICAL_SKILLS
    }

    # Sort by importance
    sorted_technical = sorted(
        technical_skill_stats.items(),
        key=lambda x: x[1]['frequency_pct'] * x[1]['avg_score'],
        reverse=True
    )

    # Identify STRENGTHS
    strengths = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] >= 70 and stats['frequency_pct'] >= 0.10:
            strengths.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Identify WEAKNESSES
    weaknesses = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] < 65 and stats['frequency_pct'] >= 0.10:
            weaknesses.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Generate cluster label
    label_parts = []

    # Add top 3 technical strengths
    for strength in strengths[:3]:
        skill_name = strength['skill'].replace(' ', '').replace('.', '').replace('-', '').title()
        label_parts.append(f"Strong{skill_name}")

    # Add top 2 weaknesses
    for weakness in weaknesses[:2]:
        skill_name = weakness['skill'].replace(' ', '').replace('.', '').replace('-', '').title()
        label_parts.append(f"Weak{skill_name}")

    cluster_label = "_".join(label_parts) if label_parts else f"MixedProfile{cluster_id}"

    # Calculate domain scores
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

    domain_scores = {}
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

    # Create cluster object
    cluster_obj = {
        'cluster_id': f"cluster_{cluster_id}",
        'cluster_label': cluster_label,
        'size': len(member_ids),
        'primary_members': len(primary_members_idx),
        'secondary_members': len(all_members_idx) - len(primary_members_idx),
        'profile': {
            'strengths': strengths[:5],
            'weaknesses': weaknesses[:5],
            'domain_scores': domain_scores,
            'profile_type': profile_type,
            'breadth': num_domains
        },
        'members': member_ids
    }

    clusters_output.append(cluster_obj)

    print(f"     Label: {cluster_label}")
    print(f"     Profile: {profile_type}")
    print(f"     Strengths: {[s['skill'] for s in strengths[:3]]}")

# Save results
print("\n9. Saving results...")

output = {
    'total_candidates': len(candidates_normalized),
    'total_clusters': len(clusters_output),
    'clustering_method': 'Skill-Weighted Multi-label GMM (v5)',
    'embedding_dims': features_umap.shape[1],
    'improvements': [
        'Solution 1: Skill-weighted assignment (50% distance + 50% skill overlap)',
        'Solution 2: Post-assignment filtering (require >= 2/5 cluster skills)',
        'Addresses misalignment between assignment and cluster labels',
        'Better interpretability and trustworthiness'
    ],
    'parameters': {
        'distance_weight': 0.5,
        'skill_weight': 0.5,
        'distance_multiplier': 1.5,
        'min_skill_matches': 2,
        'top_k_skills_checked': 5,
        'min_score_threshold': 65
    },
    'multi_label_statistics': multi_label_stats,
    'filter_statistics': {
        'passed_all_checks': filter_stats['passed_all'],
        'reassigned': filter_stats['reassigned'],
        'became_outliers': filter_stats['became_outliers'],
        'failed_checks_count': len(filter_stats['failed_checks'])
    },
    'assignment_statistics': {
        'avg_skill_overlap_score': float(np.mean(assignment_stats['avg_skill_score'])),
        'avg_distance_score': float(np.mean(assignment_stats['avg_distance_score'])),
        'avg_final_score': float(np.mean(assignment_stats['avg_final_score']))
    },
    'clusters': clusters_output
}

with open('data/processed/clusters_final_v5.json', 'w', encoding='utf-8') as f:
    json.dump(output, f, indent=2, ensure_ascii=False)

print(f"   [OK] Saved to data/processed/clusters_final_v5.json")

# Save cluster assignments
with open('data/processed/cluster_assignments_v5.json', 'w', encoding='utf-8') as f:
    json.dump(candidate_clusters, f, indent=2)

print(f"   [OK] Saved cluster assignments")

# Summary
print("\n" + "="*80)
print("PHASE 3 v5 COMPLETE - SKILL-WEIGHTED CLUSTERING + FILTERING")
print("="*80)

print(f"\nClustering Results:")
print(f"  - Total candidates: {len(candidates_normalized)}")
print(f"  - Total clusters: {len(clusters_output)}")
print(f"  - Outliers: {multi_label_stats['candidates_in_0_clusters']}")
print(f"  - Candidates in 1 cluster: {multi_label_stats['candidates_in_1_cluster']}")
print(f"  - Candidates in 2+ clusters: {multi_label_stats['candidates_in_2plus_clusters']}")

print(f"\nImprovement Metrics:")
print(f"  - Avg skill overlap score: {output['assignment_statistics']['avg_skill_overlap_score']:.3f}")
print(f"  - Candidates passed all checks: {filter_stats['passed_all']}/{len(candidates_normalized)} ({filter_stats['passed_all']/len(candidates_normalized)*100:.1f}%)")
print(f"  - Candidates reassigned: {filter_stats['reassigned']}")

print(f"\nCluster Distribution:")
for cluster in clusters_output:
    size_info = f"{cluster['size']:3} members ({cluster['primary_members']} primary)"
    print(f"  {cluster['cluster_id']:15} | {size_info:35} | {cluster['cluster_label']}")

print("\n" + "="*80)
print("IMPROVEMENTS APPLIED:")
print("  ✓ Skill-weighted assignment ensures candidates match cluster skills")
print("  ✓ Post-filtering removes misaligned assignments")
print("  ✓ Better interpretability and trustworthiness")
print("="*80)
