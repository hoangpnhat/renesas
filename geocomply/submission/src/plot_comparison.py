"""
Plot comparison charts for all ML models.
"""

import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
import json
import os

# Set style
sns.set_style("whitegrid")
plt.rcParams['figure.figsize'] = (12, 8)
plt.rcParams['font.size'] = 10


def load_results():
    """Load all model results."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(os.path.dirname(script_dir), 'data')

    results = {}

    # Baseline models
    results['Rules'] = {'precision': 0.917, 'recall': 0.834, 'f1': 0.874}
    results['RF (Original)'] = {'precision': 1.000, 'recall': 0.216, 'f1': 0.356}

    # Improved model
    results['XGBoost (Improved)'] = {'precision': 0.873, 'recall': 0.898, 'f1': 0.886}

    # Advanced models
    try:
        with open(os.path.join(data_dir, 'advanced_models_comparison.json'), 'r') as f:
            advanced = json.load(f)

        results['XGBoost + Sequential'] = advanced.get('sequential', {})
        results['Ensemble (3 models)'] = advanced.get('ensemble', {})
        results['LSTM'] = advanced.get('lstm', {})
    except FileNotFoundError:
        print("Advanced models not yet trained. Run: python src/advanced_models.py")

    return results


def plot_metrics_comparison(results, save_path):
    """Bar chart comparing all metrics."""
    models = list(results.keys())
    precisions = [results[m]['precision'] for m in models]
    recalls = [results[m]['recall'] for m in models]
    f1s = [results[m]['f1'] for m in models]

    x = np.arange(len(models))
    width = 0.25

    fig, ax = plt.subplots(figsize=(14, 8))

    bars1 = ax.bar(x - width, precisions, width, label='Precision', color='#3498db', alpha=0.8)
    bars2 = ax.bar(x, recalls, width, label='Recall', color='#2ecc71', alpha=0.8)
    bars3 = ax.bar(x + width, f1s, width, label='F1 Score', color='#e74c3c', alpha=0.8)

    ax.set_xlabel('Models', fontsize=12, fontweight='bold')
    ax.set_ylabel('Score', fontsize=12, fontweight='bold')
    ax.set_title('Performance Comparison: All Models', fontsize=14, fontweight='bold', pad=20)
    ax.set_xticks(x)
    ax.set_xticklabels(models, rotation=45, ha='right')
    ax.legend(loc='lower right', fontsize=11)
    ax.set_ylim(0, 1.05)
    ax.grid(axis='y', alpha=0.3)

    # Add value labels on bars
    def add_labels(bars):
        for bar in bars:
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{height:.3f}',
                   ha='center', va='bottom', fontsize=8)

    add_labels(bars1)
    add_labels(bars2)
    add_labels(bars3)

    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"[OK] Saved: {save_path}")
    plt.close()


def plot_f1_progression(results, save_path):
    """Line chart showing F1 improvement progression."""
    # Order models by development timeline
    timeline = [
        'RF (Original)',
        'Rules',
        'XGBoost (Improved)',
        'Ensemble (3 models)',
        'XGBoost + Sequential',
        'LSTM'
    ]

    models = [m for m in timeline if m in results]
    f1_scores = [results[m]['f1'] for m in models]

    fig, ax = plt.subplots(figsize=(12, 6))

    ax.plot(models, f1_scores, marker='o', linewidth=2, markersize=10,
            color='#e74c3c', label='F1 Score')

    # Highlight best models
    best_indices = [i for i, f1 in enumerate(f1_scores) if f1 >= 0.95]
    if best_indices:
        best_models = [models[i] for i in best_indices]
        best_f1s = [f1_scores[i] for i in best_indices]
        ax.scatter(best_models, best_f1s, s=200, color='gold', zorder=5,
                  edgecolors='black', linewidths=2, label='Best Models (F1 ≥ 95%)')

    ax.set_xlabel('Model Evolution', fontsize=12, fontweight='bold')
    ax.set_ylabel('F1 Score', fontsize=12, fontweight='bold')
    ax.set_title('Model Performance Progression', fontsize=14, fontweight='bold', pad=20)
    ax.set_ylim(0.3, 1.0)
    ax.grid(True, alpha=0.3)
    ax.legend(loc='lower right', fontsize=11)

    # Add value labels
    for i, (model, f1) in enumerate(zip(models, f1_scores)):
        ax.text(i, f1 + 0.02, f'{f1:.3f}', ha='center', fontsize=9, fontweight='bold')

    plt.xticks(rotation=45, ha='right')
    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"[OK] Saved: {save_path}")
    plt.close()


def plot_precision_recall_tradeoff(results, save_path):
    """Scatter plot: Precision vs Recall."""
    fig, ax = plt.subplots(figsize=(10, 10))

    # Separate baseline and advanced models
    baseline_models = ['Rules', 'RF (Original)', 'XGBoost (Improved)']
    advanced_models = ['Ensemble (3 models)', 'XGBoost + Sequential', 'LSTM']

    colors = {
        'baseline': '#95a5a6',
        'advanced': '#e74c3c'
    }

    # Plot baseline models
    for model in baseline_models:
        if model in results:
            r = results[model]
            ax.scatter(r['recall'], r['precision'], s=200, alpha=0.7,
                      color=colors['baseline'], edgecolors='black', linewidths=2)
            ax.text(r['recall'], r['precision'] + 0.02, model,
                   ha='center', fontsize=9, fontweight='bold')

    # Plot advanced models
    for model in advanced_models:
        if model in results:
            r = results[model]
            ax.scatter(r['recall'], r['precision'], s=300, alpha=0.8,
                      color=colors['advanced'], edgecolors='black', linewidths=2)
            ax.text(r['recall'], r['precision'] + 0.02, model,
                   ha='center', fontsize=9, fontweight='bold')

    # Add F1 contour lines
    recall_range = np.linspace(0.2, 1.0, 100)
    for f1 in [0.7, 0.8, 0.9, 0.95]:
        precision_range = f1 * recall_range / (2 * recall_range - f1)
        ax.plot(recall_range, precision_range, 'k--', alpha=0.2, linewidth=1)
        # Label F1 contours
        ax.text(0.95, f1 * 0.95 / (2 * 0.95 - f1), f'F1={f1}',
               fontsize=8, alpha=0.5)

    ax.set_xlabel('Recall (Sensitivity)', fontsize=12, fontweight='bold')
    ax.set_ylabel('Precision (Positive Predictive Value)', fontsize=12, fontweight='bold')
    ax.set_title('Precision-Recall Trade-off', fontsize=14, fontweight='bold', pad=20)
    ax.set_xlim(0.2, 1.0)
    ax.set_ylim(0.2, 1.0)
    ax.grid(True, alpha=0.3)

    # Legend
    from matplotlib.lines import Line2D
    legend_elements = [
        Line2D([0], [0], marker='o', color='w', markerfacecolor=colors['baseline'],
               markersize=10, label='Baseline Models'),
        Line2D([0], [0], marker='o', color='w', markerfacecolor=colors['advanced'],
               markersize=10, label='Advanced Models')
    ]
    ax.legend(handles=legend_elements, loc='lower left', fontsize=11)

    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"[OK] Saved: {save_path}")
    plt.close()


def plot_confusion_matrices(results, save_path):
    """Heatmap of confusion matrices for top models."""
    # Select top 4 models
    top_models = ['Rules', 'XGBoost (Improved)', 'XGBoost + Sequential', 'LSTM']
    top_models = [m for m in top_models if m in results]

    # Confusion matrices (from test results)
    cms = {
        'Rules': [[3982, 66], [145, 729]],
        'XGBoost (Improved)': [[3934, 114], [89, 785]],
        'XGBoost + Sequential': [[3979, 69], [11, 863]],
        'LSTM': [[2367, 21], [26, 508]]
    }

    fig, axes = plt.subplots(2, 2, figsize=(12, 10))
    axes = axes.flatten()

    for idx, model in enumerate(top_models):
        if model not in cms:
            continue

        cm = np.array(cms[model])
        ax = axes[idx]

        # Normalize for percentages
        cm_norm = cm.astype('float') / cm.sum(axis=1)[:, np.newaxis]

        # Plot heatmap
        sns.heatmap(cm, annot=True, fmt='d', cmap='Blues', ax=ax,
                   cbar=False, square=True, linewidths=2,
                   annot_kws={'size': 12, 'weight': 'bold'})

        # Add percentage annotations
        for i in range(2):
            for j in range(2):
                text = ax.text(j + 0.5, i + 0.7, f'({cm_norm[i, j]:.1%})',
                             ha='center', va='center', fontsize=9, color='gray')

        ax.set_xlabel('Predicted', fontsize=11, fontweight='bold')
        ax.set_ylabel('Actual', fontsize=11, fontweight='bold')
        ax.set_title(f'{model}\nF1 = {results[model]["f1"]:.3f}',
                    fontsize=12, fontweight='bold', pad=10)
        ax.set_xticklabels(['Legit', 'Spoof'])
        ax.set_yticklabels(['Legit', 'Spoof'], rotation=0)

    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"[OK] Saved: {save_path}")
    plt.close()


def plot_false_negatives_comparison(results, save_path):
    """Bar chart comparing false negative rates."""
    # FN counts (missed spoofs)
    fn_data = {
        'RF (Original)': (685, 874),  # (FN, Total Spoofs)
        'Rules': (145, 874),
        'XGBoost (Improved)': (89, 874),
        'Ensemble (3 models)': (60, 874),
        'LSTM': (26, 534),  # Different total due to sequences
        'XGBoost + Sequential': (11, 874)
    }

    models = [m for m in fn_data.keys() if m in results or '(Original)' in m or 'RF' in m]
    fn_counts = [fn_data[m][0] for m in models]
    fn_rates = [fn_data[m][0] / fn_data[m][1] * 100 for m in models]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

    # Plot 1: False Negative Counts
    bars1 = ax1.barh(models, fn_counts, color='#e74c3c', alpha=0.7)
    ax1.set_xlabel('Missed Spoofs (Count)', fontsize=11, fontweight='bold')
    ax1.set_title('False Negatives: Absolute Count', fontsize=12, fontweight='bold')
    ax1.invert_yaxis()

    for i, (bar, count) in enumerate(zip(bars1, fn_counts)):
        ax1.text(count + 10, i, f'{count}', va='center', fontweight='bold')

    # Plot 2: False Negative Rates
    bars2 = ax2.barh(models, fn_rates, color='#3498db', alpha=0.7)
    ax2.set_xlabel('Missed Spoof Rate (%)', fontsize=11, fontweight='bold')
    ax2.set_title('False Negatives: Percentage', fontsize=12, fontweight='bold')
    ax2.invert_yaxis()

    for i, (bar, rate) in enumerate(zip(bars2, fn_rates)):
        ax2.text(rate + 1, i, f'{rate:.1f}%', va='center', fontweight='bold')

    plt.tight_layout()
    plt.savefig(save_path, dpi=300, bbox_inches='tight')
    print(f"[OK] Saved: {save_path}")
    plt.close()


def main():
    print("=" * 60)
    print("Generating Comparison Charts")
    print("=" * 60)

    # Load results
    print("\nLoading model results...")
    results = load_results()
    print(f"  Loaded {len(results)} models")

    # Output directory
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_dir = os.path.join(os.path.dirname(script_dir), 'data')

    # Generate charts
    print("\nGenerating charts...")

    plot_metrics_comparison(results, os.path.join(data_dir, 'chart_metrics_comparison.png'))

    plot_f1_progression(results, os.path.join(data_dir, 'chart_f1_progression.png'))

    plot_precision_recall_tradeoff(results, os.path.join(data_dir, 'chart_precision_recall.png'))

    plot_confusion_matrices(results, os.path.join(data_dir, 'chart_confusion_matrices.png'))

    plot_false_negatives_comparison(results, os.path.join(data_dir, 'chart_false_negatives.png'))

    print("\n" + "=" * 60)
    print("All charts generated successfully!")
    print("=" * 60)
    print("\nGenerated files:")
    print("  - chart_metrics_comparison.png")
    print("  - chart_f1_progression.png")
    print("  - chart_precision_recall.png")
    print("  - chart_confusion_matrices.png")
    print("  - chart_false_negatives.png")


if __name__ == "__main__":
    main()
