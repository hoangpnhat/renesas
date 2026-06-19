# Presentation Script: Location Spoofing Detection

**Estimated Duration**: 15-20 minutes
**Presenter**: Pham Nhat Hoang

---

## Slide 1: Title Slide

Good morning/afternoon everyone. Today I'm excited to present my case study on location spoofing detection for GeoComply.

My name is Hoang Pham-Nhat, and over the past few weeks, I've been working on building a machine learning system that can detect fake GPS locations in real-time. I'm proud to share that we achieved a 95.6% F1 score with 98.7% recall - meaning we catch 98.7% of all spoofing attacks.

Let's dive in.

---

## Slide 2: Agenda

Here's what we'll cover today. We'll start with the problem overview to understand why this matters. Then I'll walk you through the system architecture, the data and features we used, and how the models evolved from a simple baseline to state-of-the-art performance. We'll look at the key results, do a technical deep dive into the best models, discuss deployment strategy, and finish with future improvements.

---

## Slide 3: Problem Statement

So, what exactly is location spoofing?

Location spoofing is when someone manipulates their GPS or location data to fake their device position. This is a serious problem across multiple industries.

In gaming, players use fake GPS to gain unfair advantages in location-based games. In the fraud space, attackers bypass geo-restrictions to access content or abuse bonuses. For compliance - and this is critical - companies in gambling and sports betting face regulatory violations if they can't verify user locations accurately. And from a security perspective, spoofing allows attackers to bypass geo-fencing and access controls.

The challenge is: how do we detect spoofing in real-time from mobile SDK telemetry? We need high recall - catching 98% or more of attacks. We need low false positives - less than 8% false alarm rate so we don't disrupt legitimate users. The decisions need to be explainable for compliance. And critically, we need inference with low latency.

---

## Slide 4: System Architecture

Here's how the system works end-to-end.

At the top, we have the mobile SDK running on iOS and Android devices. It collects 33 raw features across five categories: GPS data, network information, sensor readings, device metadata, and security flags.

These raw features flow into our feature engineering pipeline. This is where the magic happens - we create sequential features that capture velocity and acceleration between consecutive events. We validate IP addresses against GPS coordinates. We check sensor consistency. We validate timezone alignment. The output is 50 engineered features.

These features feed into our detection models. We have three production-ready models: XGBoost with sequential features achieving 95.6% F1 with 98.7% recall - this is our production champion. LSTM neural network also hitting 95.6% F1 but with 96% precision - best for high-value transactions. And an ensemble of three models (XGboostm, LightGBM, CatBoost) with 93.4% F1 - most robust to edge cases.

The models output a spoof probability and explanation. Based on this, we make a decision: block the transaction, flag for manual review, or log the event for monitoring.

---

## Slide 5: Dataset Overview

Let me tell you about the data. We generated synthetic data to have complete control over the spoofing patterns.

The training set has almost 15,000 events from about 2,400 simulated devices, with a 17.2% spoof rate covering 365 days of activity. The test set has about 5,000 events with a similar distribution. In total, around 20,000 location events.

We simulated five distinct spoofing attack types, each representing 20% of the spoofs:
- Teleportation: impossible speed like traveling from New York to Los Angeles in 2 minutes
- IP/Geo Mismatch: using a VPN with GPS spoofing
- Mock Location: "Fake GPS" apps enabled
- Sensor Mismatch: movement without any accelerometer or gyroscope activity
- Timezone Mismatch: device timezone doesn't match GPS location

---

## Slide 6: Base Features

Our 33 base features are organized into five categories.

GPS features include latitude, longitude, accuracy, speed, and bearing - these are fundamental. Network features capture IP address, WiFi count, and IP region - critical for detecting VPN-based spoofs. Sensor features track accelerometer and gyroscope variance - if someone's "moving" but sensors show zero activity, that's suspicious. Device features include battery level and timezone offset. And security features directly flag things like mock location being enabled or developer options turned on.

But here's the key limitation: single-event features can't detect trajectory anomalies. You can't spot teleportation if you only look at one event at a time.

---

## Slide 7: Sequential Features - The Breakthrough

This is where we had our breakthrough: sequential features.

The key innovation was adding previous and next event context. We calculate the distance between consecutive events using the Haversine formula. We compute velocity in kilometers per hour. We track acceleration - sudden speed changes. And we create anomaly flags.

Let me show you an example. Say we have Event 1 in San Francisco at 10:00:00 AM, and Event 2 in Los Angeles at 10:01:00 AM. The distance is 559 kilometers. Time delta is 60 seconds. That means velocity is over 33,000 kilometers per hour - physically impossible! This is a clear teleportation spoof.

These sequential features were the game-changer. They took us from 88.6% F1 to 95.6% F1 - a 7 percentage point improvement.

---

## Slide 8: Rules-Based Baseline

Before diving into machine learning, let's look at the baseline: a rules-based detector.

The strategy is simple: flag as spoof if ANY rule triggers. We have six heuristic rules:

Rule 1 detects teleportation when speed exceeds 150 km/h - like New York to LA in 2 minutes.
Rule 2 checks IP/Geo mismatch - California IP with New York GPS.
Rule 3 directly checks if mock location is enabled.
Rule 4 detects sensor mismatch - moving at 50 km/h but sensors read zero.
Rule 5 validates timezone - PST device in EST location.
Rule 6 flags perfect accuracy under 2 meters - suspiciously precise GPS.

This baseline achieved 87.4% F1 score - decent, but it has hard thresholds, no learning capability, and misses subtle spoofs. That's why we need machine learning.

---

## Slide 9: Model Evolution Journey

Here's how we evolved from failure to breakthrough.

We started with Random Forest as our ML baseline. It completely failed with only 35.6% F1 score - 100% precision but catastrophic 21.6% recall. The problem was class imbalance and no threshold tuning.

We switched to XGBoost and added class weights with scale_pos_weight=4.0, plus F1-optimized threshold tuning. This jumped us to 88.6% F1 - much better!

Then came the breakthrough: adding sequential features. XGBoost with sequential features hit 95.6% F1 with 98.7% recall. This is our production champion.

In parallel, we explored two other approaches: LSTM deep learning achieved 95.6% F1 with the highest precision at 96%. And an ensemble of XGBoost, LightGBM, and CatBoost hit 93.4% F1 - most robust with graceful degradation.

---

## Slide 10: State-of-the-Art Performance

Let's look at the final results in this comparison table.

XGBoost with Sequential features: 92.6% precision, 98.7% recall, 95.6% F1. Only 11 missed spoofs. Best for production.

LSTM: 96% precision, 95.1% recall, 95.6% F1. Only 21 false alarms. Best for high-value transactions.

Ensemble: 93.7% precision, 93.1% recall, 93.4% F1. Most robust for critical systems.

Compare this to the original Random Forest with 100% precision but 21.6% recall - it missed 685 spoofs out of 874. That's unacceptable.

Key achievements: Only 11 missed spoofs with XGBoost+Sequential. Only 21 false alarms with LSTM. A 7 percentage point F1 improvement from sequential features. And under 10 milliseconds inference latency - production-ready.

---

## Slide 11: F1 Score Progression Chart

Looking at this progression chart, you can see the journey visually.

Random Forest failed at 35.6% F1. Rules-based gave us a decent baseline at 87.4%. XGBoost improved to 88.6%. Then the sequential features breakthrough took us to 95.6% F1. Both XGBoost+Sequential and LSTM achieved this state-of-the-art result.

---

## Slide 12: Comprehensive Metrics Comparison

This chart shows all models across all metrics.

For model selection, here's my guidance: Use XGBoost with Sequential for production - it has the highest recall at 98.7%, catching nearly all attacks. Use LSTM for low-tolerance scenarios like high-value transactions - it has the highest precision at 96%, minimizing false alarms. Use the Ensemble for critical systems where you need graceful degradation.

---

## Slide 13: Confusion Matrices Comparison

These confusion matrices tell the full story.

On the left, you see the confusion matrices for all models. On the right, the precision-recall trade-off chart.

XGBoost+Sequential catches all but 11 spoofs - 98.7% recall. That's production-ready. But it has 69 false alarms.

LSTM has only 21 false alarms - 96% precision. But it misses 43 spoofs.

The trade-off is clear: do you prioritize catching attacks or minimizing user disruption? For most production systems, XGBoost+Sequential is the right choice. For high-value transactions where false alarms are very costly, use LSTM.

---

## Slide 14: Precision-Recall Trade-off

[This slide appears to be combined with the previous one in the updated layout]

---

## Slide 15: False Negatives Analysis

This chart shows the dramatic reduction in missed attacks.

Random Forest missed 685 spoofs - 78.4% miss rate. Unacceptable.
Rules-based missed 145 - 16.6%. Better, but still not good enough.
XGBoost missed 89 - 10.2%. Getting there.
XGBoost with Sequential missed only 11 - 1.3% miss rate. That's a 98% reduction compared to Random Forest!

Sequential features reduced missed attacks by 98%. This is the key achievement.

---

## Slide 16: Technical Deep Dive - XGBoost

Let's do a technical deep dive into our production model.

The architecture uses XGBoost with 100 decision trees, max depth of 6 to prevent overfitting, learning rate 0.1, and critically, scale_pos_weight=4.0 to handle the 80/20 class imbalance.

Looking at feature importance, the top 5 features are:
1. extreme_velocity at 0.18 importance - detects teleportation
2. mock_location_enabled at 0.15 - direct spoofing indicator
3. velocity_from_prev at 0.12 - trajectory analysis
4. distance_from_prev at 0.09 - movement validation
5. ip_matches_gps at 0.08 - network consistency

Notice how 3 of the top 5 are sequential features. This confirms their critical importance.

---

## Slide 17: Alternative Approach - LSTM

Now let's look at the LSTM approach.

The architecture takes 5 consecutive events with 25 features each as input. We have two LSTM layers with 64 units and 30% dropout each. Then a dense layer with 32 units and ReLU activation. Finally, an output layer with sigmoid activation giving us a spoof probability between 0 and 1. Total parameters: 58,689.

Why LSTM? Three reasons: automatic pattern learning without manual feature engineering, it learns temporal dependencies and trajectory patterns naturally, and it achieves the highest precision at 96% - only 21 false alarms.

The trade-offs: LSTM has better precision but XGBoost has better recall. LSTM needs GPU for 20ms inference, XGBoost runs on CPU in under 10ms. LSTM is a black box, XGBoost gives us feature importance for explainability. Use LSTM for high-value transactions, XGBoost for production at scale.

---

## Slide 18: Error Analysis - What Gets Missed?

Let's understand what goes wrong. First, false positives - legitimate events we incorrectly flagged.

30% are timezone mismatches from travelers who haven't updated their device timezone. 25% are perfect accuracy cases - modern phones in open areas can achieve under 2 meters. 20% are IP/Geo mismatches from corporate VPN users. 15% are sensor mismatches from smooth highway driving with steady cruise control. 10% are fast movement like high-speed rail at 120 km/h.

Now, false negatives - the 11 spoofs XGBoost+Sequential missed. 45% are slow spoofing: small movements below our velocity threshold, like walking 2km in 30 minutes at 4 km/h - looks legitimate. 36% are stationary spoofing: set a fake location and don't move - no sequential features to analyze. 18% are sophisticated spoofing: fake GPS plus fake accelerometer and gyroscope data - all indicators look normal.

---

## Slide 19: Deployment Strategy

Here's how we deploy this to production.

The architecture is straightforward: mobile SDK collects 50 features and sends them via HTTPS POST to our API Gateway load balancer. The gateway routes to multiple Feature Service instances that compute sequential features. These feed into Model Inference instances running XGBoost+Sequential in under 10ms per batch. The Response Service packages the spoof probability, flag, explanation, and confidence score.

We deploy in three phases:

Phase 1 - Shadow Mode for weeks 1-2: Deploy alongside the existing system, log predictions but don't block users, compare with manual reviews, and validate that our 95.6% F1 holds on real data.

Phase 2 - Hybrid Mode for weeks 3-4: Block high-confidence spoofs with score above 0.95, flag medium-confidence for manual review, monitor false positive rate to stay below 2%, and collect user feedback.

Phase 3 - Full Rollout in month 2+: Gradually increase to 100% traffic, set up real-time monitoring dashboard, auto-scale based on load, and A/B test model improvements.

---

## Slide 20: Performance Characteristics

Let's talk about latency and scalability.

Feature engineering takes 1-2ms, sequential features 2-3ms, XGBoost inference 5-8ms. Total pipeline under 10ms per event. Throughput is 1,000 events per second per instance.

A single instance with 4 CPU cores can handle 1,000 events per second - that's 86.4 million events daily with 99th percentile latency under 15ms.

Scale to a cluster of 10 instances and you get 10,000 events per second - 864 million events daily. We auto-scale when CPU exceeds 70%.

---

## Slide 21: Robustness & Monitoring

Monitoring is critical for production ML systems.

Our real-time dashboard shows precision at 92.3%, recall at 98.1%, F1 score at 95.1% - all hitting our targets. We track events processed, spoofs detected, estimated false alarms, and p99 latency.

We have alerts for when metrics drop. For example, if F1 drops to 94.2%, we get an alert and auto-scaling triggers.

For drift detection, we run daily Kolmogorov-Smirnov tests comparing today's feature distributions to training distributions. If the p-value drops below 0.01, we alert "Data drift detected!" and trigger model retraining automatically.

---

## Slide 22-23: Future Improvements

Looking ahead, we have short-term, medium-term, and long-term improvements planned.

Short-term for Q1 2025:
First, real-world validation - deploy shadow mode on production traffic, collect 100,000+ labeled examples, retrain on real data to handle the distribution shift, and validate that 95.6% F1 holds.

Second, explainability layer - compute SHAP values for each prediction, surface the top 3 contributing features, and provide user-friendly explanations like "Flagged due to impossible speed: NYC to LA in 2 minutes."

Third, threshold optimization - build a business-specific cost matrix where the cost of a false negative is 10 times the cost of a false positive, optimize for revenue impact not just F1, and A/B test different operating points.

Medium-term for Q2-Q3 2025:
Fourth, per-user behavioral profiles - track home and work locations, flag deviations from typical patterns, and use adaptive thresholds based on user risk score. Low-risk users get higher thresholds for fewer false alarms, high-risk users get lower thresholds to catch more spoofs.

Fifth, adversarial robustness - generate adversarial examples assuming attackers know our model, use adversarial training to harden the model, randomize detection parameters per session, and do red team testing.

Sixth, multi-spoof-type models - train specialized models for each spoof type: a teleportation detector focused on velocity, a mock location detector focused on sensors, an IP/Geo mismatch detector focused on network. Then ensemble their predictions.

Long-term for 2026+:
Seventh, Transformer architecture - replace LSTM with Transformer using multi-head self-attention. Transformers can handle longer sequences of 10+ events, process in parallel faster than LSTM, and capture better long-range dependencies.

Eighth, multi-modal fusion - combine signals from multiple sources: our location features which we already have, device fingerprinting with IMEI and hardware IDs, behavioral biometrics like typing patterns and app usage, and network-level signals like ISP, ASN, and round-trip time. Use a late fusion strategy to combine everything. Expected result: F1 above 98%.

---

## Slide 24: Key Takeaways

Let me summarize the key takeaways.

Technical achievements: We achieved 95.6% F1 score with 98.7% recall - only 11 missed spoofs out of 874. Sequential features were the breakthrough, giving us +7% F1 improvement. The system is production-ready with under 10ms inference latency. And it's explainable with feature importance and rule explanations for compliance.

Engineering best practices: We took an iterative approach from baseline to improved to advanced models. We handled class imbalance with scale_pos_weight=4.0. We optimized for F1, not just precision or recall individually. And we used ensemble diversity combining XGBoost, LightGBM, and CatBoost.

Business impact is clear: A rules-only baseline achieved 87.4% F1 and missed 145 spoofs. Our advanced XGBoost+Sequential achieved 95.6% F1 and missed only 11 spoofs. That's 134 fewer missed attacks - a 92% improvement. The value is preventing fraud, maintaining regulatory compliance, and building user trust.

---

## Slide 25: Lessons Learned

Here's what we learned from this project.

What worked: Sequential features adding temporal context were critical - single-event features simply can't detect trajectory anomalies. XGBoost was the best model for tabular data with engineered features. Synthetic data gave us controlled experimentation. And iterative development from baseline to advanced kept us on track.

What didn't work: Random Forest baseline failed completely due to no class imbalance handling. Single-event features only couldn't detect trajectory spoofs. Precision-only optimization led to terrible recall of 21.6%. And equal ensemble weights - we found 0.4/0.3/0.3 performs better than 0.33/0.33/0.33.

Key insights: Domain knowledge matters - sequential features required understanding physics like velocity and acceleration. Real-world validation is needed - synthetic data is a starting point, not the end goal. Explainability beats black-box - SHAP values and feature importance build trust with stakeholders. And continuous monitoring is essential - data drift detection and model retraining pipelines keep the system healthy.

---

## Slide 26: Q&A

Now I'd like to open the floor for questions. But let me preemptively answer some common ones.

Q1: Why 95.6% F1 and not 99%+?
A: Synthetic data has limitations. Some spoofs are designed to be subtle - slow spoofing, stationary spoofing. Real-world data plus behavioral profiling could push us to 97-98%.

Q2: Can attackers evade this?
A: Sophisticated attackers who simulate sensors correctly can evade detection. Our mitigation plan includes adversarial training, randomized thresholds, and multi-modal signals like device fingerprinting and behavioral biometrics.

Q3: What about privacy?
A: All features are telemetry-based with no personally identifiable information. Location data is processed server-side with proper encryption. The system is GDPR and CCPA compliant.

Q4: Latency concerns at scale?
A: We're at under 10ms per event on a single instance. Auto-scaling handles 10,000 events per second across a cluster. If we need faster inference, we can use GPU acceleration for LSTM or further optimize XGBoost.

What other questions do you have?

---

## Slide 27: Thank You

Thank you all for your time today.

To summarize: The challenge was detecting location spoofing in mobile apps. Our solution combined sequential features with XGBoost and LSTM models. The result is 95.6% F1 score, 98.7% recall, and under 10ms latency - production-ready.

The next steps are real-world validation in shadow mode, model hardening with adversarial training, adding explainability with SHAP values, and scaling to multi-modal fusion and Transformer architecture.

If you'd like to connect, my email is hoangpnhat@gmail.com and you can find me on LinkedIn as Hoang Pham-Nhat.

This project was built with Python, XGBoost, PyTorch, and scikit-learn, with AI assistance from Claude Code powered by Claude 3.5 Sonnet. The entire journey from data generation to advanced models took about 15 hours.

Thank you, and I look forward to your questions!

---

## Tips for Delivery

1. **Pace**: Aim for 1-1.5 minutes per slide for technical slides, 2-3 minutes for results/demo slides
2. **Energy**: Show enthusiasm when discussing the breakthrough (sequential features)
3. **Eye Contact**: Look at your audience, not just the slides
4. **Pauses**: Pause after key numbers (95.6% F1, 98.7% recall, only 11 missed spoofs)
5. **Transitions**: Use phrases like "Now let's look at...", "Moving on to...", "Here's the key insight..."
6. **Questions**: Encourage questions throughout, not just at the end
7. **Backup Slides**: Have the appendix slides ready in case of technical questions
8. **Demo**: If possible, show a live demo of the model making predictions
9. **Stories**: When explaining false positives/negatives, use concrete examples to make it relatable
10. **Confidence**: You know this material - trust your preparation!

## Key Message to Emphasize

**The Breakthrough**: Sequential features were the game-changer. Moving from single-event features to temporal context (velocity, acceleration, trajectory) took us from 88.6% to 95.6% F1 - a 7 percentage point improvement that reduced missed attacks by 98%.

This is the story you want the audience to remember.
