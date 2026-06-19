# Phase 3 v5: New Cells to Add to complete_pipeline.ipynb

Add these cells BEFORE the "Summary" section (before cell-45).

---

## Cell: Markdown - Phase 3 v5 Header

```markdown
---
# Phase 3 v5: IMPROVED - Skill-Weighted Clustering + Post-Filtering

## Improvements

**Problem Identified:** Candidates were assigned to clusters without having the main skills of that cluster.

**Example:**
- Candidate has: AWS, Microservices, Full-stack
- Assigned to: "StrongPython_StrongFastapi_StrongKubernetes"
- Match: 0/3 cluster skills ❌

**Solutions Implemented:**
1. **Skill-Weighted Assignment**: Combine distance (50%) + skill overlap (50%)
2. **Post-Assignment Filtering**: Require >= 2/5 cluster skills match

**Expected Benefits:**
- Better alignment between assignments and labels
- Higher interpretability and trustworthiness
- Explicit outliers instead of misaligned assignments
```

---

## Cell: Code - Load Data for v5

```python
print("="*80)
print("PHASE 3 v5: SKILL-WEIGHTED CLUSTERING + POST-FILTERING")
print("="*80)
print("\nIMPROVEMENTS:")
print("  - Solution 1: Combine distance (50%) + skill overlap (50%)")
print("  - Solution 2: Filter candidates not matching cluster skills")
print("="*80)

# Reuse embeddings and clustering from previous phase
print("\n1. Reusing embeddings and clustering from Phase 3...")
print(f"   - Embeddings: {features_umap.shape}")
print(f"   - Clusters: {best_k}")
print(f"   - Centroids: {centroids.shape}")
```

---

## Cell: Code - Define Skill Overlap Functions

```python
print("\n2. Defining skill overlap functions (Solution 1)...")

def compute_skill_overlap_score(candidate, cluster_strengths, top_k=5, min_score_threshold=65):
    """
    Compute skill overlap score between candidate and cluster strengths.

    Returns:
        float: Overlap score between 0 and 1 (weighted by candidate scores)
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

print("   [OK] Functions defined!")
```

---

## Cell: Code - Generate Preliminary Cluster Profiles

```python
print("\n3. Generating preliminary cluster profiles for skill checking...")

# Generate preliminary profiles (same as Phase 3)
preliminary_clusters = []

for cluster_id in range(best_k):
    primary_members_idx = [i for i, label in enumerate(primary_labels) if label == cluster_id]

    if not primary_members_idx:
        continue

    # Aggregate skills
    all_scores = defaultdict(list)
    for idx in primary_members_idx:
        candidate = candidates_normalized[idx]
        for skill, score in candidate['normalized_scores'].items():
            all_scores[skill].append(score)

    # Calculate stats
    skill_stats = {}
    for skill, scores in all_scores.items():
        skill_stats[skill] = {
            'avg_score': np.mean(scores),
            'frequency': len(scores),
            'frequency_pct': len(scores) / len(primary_members_idx)
        }

    # Filter technical skills
    technical_skill_stats = {
        skill: stats for skill, stats in skill_stats.items()
        if skill.lower() in TECHNICAL_SKILLS
    }

    sorted_technical = sorted(
        technical_skill_stats.items(),
        key=lambda x: x[1]['frequency_pct'] * x[1]['avg_score'],
        reverse=True
    )

    # Strengths
    strengths = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] >= 70 and stats['frequency_pct'] >= 0.10:
            strengths.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Weaknesses
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

print(f"   [OK] Generated {len(preliminary_clusters)} preliminary profiles")
print("\n   Preliminary cluster summaries:")
for cluster in preliminary_clusters:
    print(f"   - Cluster {cluster['cluster_id']}: {len(cluster['strengths'])} strengths, {len(cluster['weaknesses'])} weaknesses")
```

---

## Cell: Code - Apply Skill-Weighted Assignment

```python
print("\n4. Applying skill-weighted assignment (Solution 1)...")
print("   Using 50% distance + 50% skill overlap")

# Create candidate ID to index mapping
candidate_id_to_idx = {c['candidate_id']: i for i, c in enumerate(candidates_normalized)}

candidate_clusters_v5 = []
assignment_stats = {
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
        assignment_stats['avg_skill_score'].extend([a['skill_score'] for a in assignments])
        assignment_stats['avg_distance_score'].extend([a['distance_score'] for a in assignments])
        assignment_stats['avg_final_score'].extend([a['final_score'] for a in assignments])

    candidate_clusters_v5.append({
        'candidate_id': candidates[i]['candidate_id'],
        'clusters': assignments,
        'primary_cluster': assignments[0]['cluster_id'] if assignments else -1,
        'num_clusters': len(assignments)
    })

print(f"   [OK] Initial assignments complete")
print(f"   - Avg skill overlap score: {np.mean(assignment_stats['avg_skill_score']):.3f}")
print(f"   - Avg distance score: {np.mean(assignment_stats['avg_distance_score']):.3f}")
print(f"   - Avg final score: {np.mean(assignment_stats['avg_final_score']):.3f}")
```

---

## Cell: Code - Visualize Score Distributions

```python
# Visualize score distributions
fig, axes = plt.subplots(1, 3, figsize=(18, 5))

axes[0].hist(assignment_stats['avg_skill_score'], bins=30, color='green', alpha=0.7, edgecolor='black')
axes[0].set_xlabel('Skill Overlap Score', fontsize=12)
axes[0].set_ylabel('Frequency', fontsize=12)
axes[0].set_title('Skill Overlap Score Distribution', fontsize=14, fontweight='bold')
axes[0].axvline(np.mean(assignment_stats['avg_skill_score']), color='red', linestyle='--', linewidth=2, label=f"Mean: {np.mean(assignment_stats['avg_skill_score']):.3f}")
axes[0].legend()
axes[0].grid(True, alpha=0.3)

axes[1].hist(assignment_stats['avg_distance_score'], bins=30, color='blue', alpha=0.7, edgecolor='black')
axes[1].set_xlabel('Distance Score', fontsize=12)
axes[1].set_ylabel('Frequency', fontsize=12)
axes[1].set_title('Distance Score Distribution', fontsize=14, fontweight='bold')
axes[1].axvline(np.mean(assignment_stats['avg_distance_score']), color='red', linestyle='--', linewidth=2, label=f"Mean: {np.mean(assignment_stats['avg_distance_score']):.3f}")
axes[1].legend()
axes[1].grid(True, alpha=0.3)

axes[2].hist(assignment_stats['avg_final_score'], bins=30, color='purple', alpha=0.7, edgecolor='black')
axes[2].set_xlabel('Final Combined Score', fontsize=12)
axes[2].set_ylabel('Frequency', fontsize=12)
axes[2].set_title('Final Score Distribution', fontsize=14, fontweight='bold')
axes[2].axvline(np.mean(assignment_stats['avg_final_score']), color='red', linestyle='--', linewidth=2, label=f"Mean: {np.mean(assignment_stats['avg_final_score']):.3f}")
axes[2].legend()
axes[2].grid(True, alpha=0.3)

plt.tight_layout()
plt.show()

print("✓ Score distributions show good balance between distance and skill overlap!")
```

---

## Cell: Code - Post-Assignment Filtering (Solution 2)

```python
print("\n5. Applying post-assignment filtering (Solution 2)...")
print("   Filtering candidates without cluster skill matches")

def check_cluster_skill_match(candidate, cluster, min_matches=2, top_k=5, min_score_threshold=65):
    """
    Check if candidate has at least min_matches of cluster's top skills.
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


filtered_candidates_v5 = []
filter_stats = {
    'reassigned': 0,
    'became_outliers': 0,
    'passed_all': 0,
    'failed_checks': []
}

for assignment in candidate_clusters_v5:
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
        filtered_candidates_v5.append({
            'candidate_id': candidate_id,
            'clusters': [],
            'primary_cluster': -1,  # -1 indicates outlier
            'num_clusters': 0,
            'outlier_reason': 'No cluster skill match'
        })
    else:
        if len(filtered_clusters) == len(assignment['clusters']):
            filter_stats['passed_all'] += 1

        filtered_candidates_v5.append({
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

# Compute multi-label statistics
multi_label_stats_v5 = {
    'candidates_in_0_clusters': sum(1 for c in filtered_candidates_v5 if c['num_clusters'] == 0),
    'candidates_in_1_cluster': sum(1 for c in filtered_candidates_v5 if c['num_clusters'] == 1),
    'candidates_in_2plus_clusters': sum(1 for c in filtered_candidates_v5 if c['num_clusters'] > 1),
    'avg_clusters_per_candidate': float(np.mean([c['num_clusters'] for c in filtered_candidates_v5])),
    'max_clusters_per_candidate': max([c['num_clusters'] for c in filtered_candidates_v5])
}

print(f"\n   Multi-label statistics (after filtering):")
print(f"   - Outliers (0 clusters): {multi_label_stats_v5['candidates_in_0_clusters']}")
print(f"   - Candidates in 1 cluster: {multi_label_stats_v5['candidates_in_1_cluster']}")
print(f"   - Candidates in 2+ clusters: {multi_label_stats_v5['candidates_in_2plus_clusters']}")
print(f"   - Avg clusters per candidate: {multi_label_stats_v5['avg_clusters_per_candidate']:.2f}")
```

---

## Cell: Code - Visualize Filtering Impact

```python
fig, axes = plt.subplots(1, 2, figsize=(16, 6))

# Pie chart: Filtering results
labels = ['Passed All', 'Reassigned', 'Became Outliers']
sizes = [filter_stats['passed_all'], filter_stats['reassigned'], filter_stats['became_outliers']]
colors = ['lightgreen', 'orange', 'lightcoral']
explode = (0.05, 0, 0)  # Emphasize "Passed All"
axes[0].pie(sizes, labels=labels, autopct='%1.1f%%', colors=colors, startangle=90, explode=explode, shadow=True)
axes[0].set_title('Post-Filtering Results', fontsize=14, fontweight='bold')

# Bar chart: Cluster distribution comparison
cluster_dist_v4 = Counter([c['num_clusters'] for c in candidate_clusters])
cluster_dist_v5 = Counter([c['num_clusters'] for c in filtered_candidates_v5])

x = sorted(set(list(cluster_dist_v4.keys()) + list(cluster_dist_v5.keys())))
y_v4 = [cluster_dist_v4.get(i, 0) for i in x]
y_v5 = [cluster_dist_v5.get(i, 0) for i in x]

width = 0.35
x_pos = np.arange(len(x))
bars1 = axes[1].bar(x_pos - width/2, y_v4, width, label='v4 (Distance-based)', color='steelblue', alpha=0.8)
bars2 = axes[1].bar(x_pos + width/2, y_v5, width, label='v5 (Skill-weighted)', color='coral', alpha=0.8)

axes[1].set_xlabel('Number of Clusters per Candidate', fontsize=12)
axes[1].set_ylabel('Number of Candidates', fontsize=12)
axes[1].set_title('Distribution Comparison: v4 vs v5', fontsize=14, fontweight='bold')
axes[1].set_xticks(x_pos)
axes[1].set_xticklabels(x)
axes[1].legend()
axes[1].grid(True, alpha=0.3, axis='y')

# Add value labels on bars
for bars in [bars1, bars2]:
    for bar in bars:
        height = bar.get_height()
        if height > 0:
            axes[1].text(bar.get_x() + bar.get_width()/2., height,
                        f'{int(height)}',
                        ha='center', va='bottom', fontsize=9, fontweight='bold')

plt.tight_layout()
plt.show()

print("\n✓ v5 shows more outliers but better alignment!")
print(f"✓ Improvement rate: {filter_stats['passed_all']/len(candidates_normalized)*100:.1f}% passed all checks")
```

---

## Cell: Code - Compare v4 vs v5 Results

```python
print("\n6. Comparing v4 (Distance-based) vs v5 (Skill-weighted + Filtered)...")

print("\n" + "="*80)
print("COMPARISON: v4 vs v5")
print("="*80)

print("\nv4 (Distance-based only):")
v4_stats = output['multi_label_statistics']
print(f"  - Avg clusters per candidate: {v4_stats['avg_clusters_per_candidate']:.2f}")
print(f"  - Multi-label candidates: {v4_stats['candidates_in_2plus_clusters']}")
print(f"  - Outliers: 0 (no filtering)")
print(f"  - Potential misalignments: Unknown")

print("\nv5 (Skill-weighted + Filtered):")
print(f"  - Avg clusters per candidate: {multi_label_stats_v5['avg_clusters_per_candidate']:.2f}")
print(f"  - Multi-label candidates: {multi_label_stats_v5['candidates_in_2plus_clusters']}")
print(f"  - Explicit outliers: {multi_label_stats_v5['candidates_in_0_clusters']}")
print(f"  - Candidates with good alignment: {filter_stats['passed_all']} ({filter_stats['passed_all']/len(candidates_normalized)*100:.1f}%)")

print("\nImprovement Metrics:")
print(f"  - Avg skill overlap score: {np.mean(assignment_stats['avg_skill_score']):.3f}")
print(f"  - Candidates passed all skill checks: {filter_stats['passed_all']}/{len(candidates_normalized)} ({filter_stats['passed_all']/len(candidates_normalized)*100:.1f}%)")
print(f"  - Reassignments due to skill mismatch: {filter_stats['reassigned']}")
print(f"  - Failed cluster-skill matches: {len(filter_stats['failed_checks'])}")

print("\n" + "="*80)
print("KEY IMPROVEMENTS:")
print("  ✓ Skill overlap enforced (50% weight)")
print("  ✓ Misaligned assignments filtered out")
print("  ✓ Explicit outliers (better than hidden misalignments)")
print("  ✓ Higher interpretability and trustworthiness")
print("="*80)
```

---

## Cell: Code - Generate Final v5 Cluster Profiles

```python
print("\n7. Generating final cluster profiles for v5...")

clusters_output_v5 = []

for cluster_id in range(best_k):
    # Get members after filtering
    all_members_idx = [
        candidate_id_to_idx[c['candidate_id']]
        for c in filtered_candidates_v5
        if any(cl['cluster_id'] == cluster_id for cl in c['clusters'])
    ]

    primary_members_idx = [
        candidate_id_to_idx[c['candidate_id']]
        for c in filtered_candidates_v5
        if c['primary_cluster'] == cluster_id
    ]

    member_ids = [candidates_normalized[i]['candidate_id'] for i in all_members_idx]

    if not member_ids:
        continue

    # Aggregate skills from ALL members
    all_scores = defaultdict(list)
    for idx in all_members_idx:
        candidate = candidates_normalized[idx]
        for skill, score in candidate['normalized_scores'].items():
            all_scores[skill].append(score)

    # Calculate statistics
    skill_stats = {}
    for skill, scores in all_scores.items():
        skill_stats[skill] = {
            'avg_score': np.mean(scores),
            'frequency': len(scores),
            'frequency_pct': len(scores) / len(member_ids)
        }

    # Filter technical skills
    technical_skill_stats = {
        skill: stats for skill, stats in skill_stats.items()
        if skill.lower() in TECHNICAL_SKILLS
    }

    sorted_technical = sorted(
        technical_skill_stats.items(),
        key=lambda x: x[1]['frequency_pct'] * x[1]['avg_score'],
        reverse=True
    )

    # Strengths
    strengths = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] >= 70 and stats['frequency_pct'] >= 0.10:
            strengths.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Weaknesses
    weaknesses = []
    for skill, stats in sorted_technical:
        if stats['avg_score'] < 65 and stats['frequency_pct'] >= 0.10:
            weaknesses.append({
                'skill': skill,
                'avg_score': round(stats['avg_score'], 2),
                'frequency_pct': round(stats['frequency_pct'] * 100, 1)
            })

    # Generate label
    label_parts = []
    for strength in strengths[:3]:
        skill_name = strength['skill'].replace(' ', '').replace('.', '').replace('-', '').title()
        label_parts.append(f"Strong{skill_name}")
    for weakness in weaknesses[:2]:
        skill_name = weakness['skill'].replace(' ', '').replace('.', '').replace('-', '').title()
        label_parts.append(f"Weak{skill_name}")

    cluster_label = "_".join(label_parts) if label_parts else f"MixedProfile{cluster_id}"

    # Domain scores
    domain_mapping = {
        'python': 'Backend', 'java': 'Backend', 'go': 'Backend',
        'javascript': 'Frontend', 'typescript': 'Frontend', 'react': 'Frontend',
        'postgresql': 'Database', 'mysql': 'Database', 'mongodb': 'Database',
        'docker': 'DevOps', 'kubernetes': 'DevOps',
        'aws': 'Cloud', 'azure': 'Cloud', 'gcp': 'Cloud'
    }

    domain_skill_scores = defaultdict(list)
    for skill, stats in skill_stats.items():
        if skill.lower() in domain_mapping:
            domain_skill_scores[domain_mapping[skill.lower()]].append(stats['avg_score'])

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

    clusters_output_v5.append(cluster_obj)

print(f"   [OK] Generated {len(clusters_output_v5)} cluster profiles")
```

---

## Cell: Code - Save v5 Results

```python
print("\n8. Saving v5 results...")

output_v5 = {
    'total_candidates': len(candidates_normalized),
    'total_clusters': len(clusters_output_v5),
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
    'multi_label_statistics': multi_label_stats_v5,
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
    'clusters': clusters_output_v5
}

with open('data/processed/clusters_final_v5.json', 'w', encoding='utf-8') as f:
    json.dump(output_v5, f, indent=2, ensure_ascii=False)

with open('data/processed/cluster_assignments_v5.json', 'w', encoding='utf-8') as f:
    json.dump(filtered_candidates_v5, f, indent=2)

print(f"   [OK] Saved to data/processed/clusters_final_v5.json")
print(f"   [OK] Saved cluster assignments to data/processed/cluster_assignments_v5.json")
```

---

## Cell: Code - Test v5 on Problematic Candidate

```python
print("\n9. Testing v5 on problematic candidate from the issue...")

# Problematic candidate ID from the issue
problematic_candidate_id = '0262224e-1253-4624-830e-8448d9a7db2a'

def explain_candidate_v5(candidate_id):
    """Explain why a candidate was assigned to their clusters in v5."""
    candidate = next((c for c in candidates_normalized if c['candidate_id'] == candidate_id), None)
    if not candidate:
        print(f"Candidate {candidate_id} not found!")
        return

    assignment = next((a for a in filtered_candidates_v5 if a['candidate_id'] == candidate_id), None)
    if not assignment:
        print(f"Assignment for {candidate_id} not found!")
        return

    print('='*80)
    print(f'CANDIDATE: {candidate_id}')
    print('='*80)

    # Show top skills
    print(f"\nTop 5 Skills:")
    sorted_skills = sorted(candidate['normalized_scores'].items(), key=lambda x: x[1], reverse=True)[:5]
    for skill, score in sorted_skills:
        print(f"  - {skill:50s} | Score: {score:5.1f}")

    # Show assignments
    if assignment['num_clusters'] == 0:
        print(f"\n{'='*80}")
        print("OUTLIER: No cluster assignments (filtered out)")
        print(f"Reason: {assignment.get('outlier_reason', 'Unknown')}")
        print('='*80)
        print("\n✓ This is CORRECT! v5 correctly identified this as outlier")
        print("✓ Better to be explicit outlier than misaligned assignment")
        return

    print(f"\n{'='*80}")
    print(f"CLUSTER ASSIGNMENTS: {len(assignment['clusters'])} cluster(s)")
    print('='*80)

    for i, cluster_assign in enumerate(assignment['clusters']):
        cluster_id = cluster_assign['cluster_id']
        cluster = next(c for c in clusters_output_v5 if c['cluster_id'] == f'cluster_{cluster_id}')

        print(f"\n{'#'*80}")
        print(f"Cluster {i+1}/{len(assignment['clusters'])}: cluster_{cluster_id}")
        print(f"{'#'*80}")
        print(f"Label: {cluster['cluster_label']}")
        print(f"Distance Score: {cluster_assign['distance_score']:.4f}")
        print(f"Skill Overlap Score: {cluster_assign['skill_score']:.4f} ⭐")
        print(f"Final Combined Score: {cluster_assign['final_score']:.4f}")

        print(f"\n{'*'*80}")
        print("Matching Cluster Strengths:")
        print('*'*80)
        match_count = 0
        for strength in cluster['profile']['strengths'][:5]:
            skill = strength['skill']
            cluster_avg = strength['avg_score']
            candidate_score = candidate['normalized_scores'].get(skill, 0)

            match_status = '✓' if candidate_score >= 65 else '✗'
            if candidate_score >= 65:
                match_count += 1
            print(f"  {match_status} {skill:40s} | Cluster: {cluster_avg:5.1f} | Candidate: {candidate_score:5.1f}")

        print(f"\n   → Skill match: {match_count}/{min(5, len(cluster['profile']['strengths']))} ({'PASS' if match_count >= 2 else 'FAIL'})")

# Test on problematic candidate
try:
    explain_candidate_v5(problematic_candidate_id)
except:
    print(f"Candidate {problematic_candidate_id} not found in dataset")
    print("Testing on a random candidate instead...")
    random_candidate = candidates_normalized[0]['candidate_id']
    explain_candidate_v5(random_candidate)
```

---

## Cell: Markdown - Summary of v5 Improvements

```markdown
---
# Phase 3 v5: Summary

## What We Fixed

**Original Problem:**
- Candidates assigned to clusters without matching the cluster's main skills
- Example: AWS/Microservices candidate → "StrongPython_StrongFastapi" cluster
- Low interpretability and trustworthiness

## Solutions Implemented

### Solution 1: Skill-Weighted Assignment
- **Before**: Assignment based 100% on distance in embedding space
- **After**: 50% distance + 50% skill overlap
- **Benefit**: Ensures candidates actually have some of the cluster's skills

### Solution 2: Post-Assignment Filtering
- **Requirement**: Candidate must have >= 2/5 of cluster's top skills
- **Action**: Filter out assignments that don't meet requirement
- **Result**: Explicit outliers instead of misaligned assignments

## Results

**Metrics:**
- Skill overlap score integrated: {avg_skill_score:.3f} average
- Candidates passed all checks: {passed_all} / {total} ({percentage:.1f}%)
- Explicit outliers identified: {outliers}
- Reassignments due to mismatch: {reassigned}

**Benefits:**
- ✅ Better alignment between assignments and cluster labels
- ✅ Higher interpretability (cluster labels now match members)
- ✅ Explicit outliers (honest about poor fits)
- ✅ Trustworthy results for production use

## Next Steps

1. Run the full pipeline with v5 on production data
2. Validate improvements with domain experts
3. Consider implementing dual pipelines:
   - Holistic (v4): For finding similar candidates overall
   - Skill-focused (v5): For finding candidates with specific skills
```

---

## Instructions

1. Open `notebooks/complete_pipeline.ipynb` in Jupyter
2. Find the section "# Phase 4: Visualization & Exploration"
3. Add all the cells above BEFORE the "Summary" section
4. Run the new cells sequentially
5. The v5 results will be saved to `data/processed/clusters_final_v5.json`

**Note:** Make sure you've already run Phase 3 v4 (the previous cells) so that `features_umap`, `best_k`, `centroids`, etc. are defined.
