# From architectural question to defensible paper

[Research program](research-program.md) · [Paper study](paper-study.md)

This is a working method for future studies. It supplies no experimental findings or novelty assessment. Use it to avoid turning a plausible design into an unsupported conclusion.

## 1 Start with a claim and an alternative explanation

State the problem, affected workload, failure/consistency contract and resource boundary. Then state the proposed contribution in a sentence that can be contradicted. Avoid an unqualified claim such as “better storage for edge.” Prefer a conditional claim about bytes, latency, recoverability or metadata under defined assumptions.

Create a claim ledger:

| Field | What to record |
|---|---|
| Claim | Precise expected result and scope |
| Evidence needed | Proof/model, measurement, implementation or experience |
| Baseline | Strongest relevant alternatives under comparable contracts |
| Rival explanation | Tuning, caching, omitted work, weaker semantics, hardware artifact |
| Falsifier | Observation that invalidates or narrows the claim |
| Status | Proposed, supported, contradicted or unresolved |

Do not fill an abstract with results before collecting them. A thought experiment is a model, not a measured outcome.

## 2 Establish prior work and the actual gap

Search by problem, mechanism and adjacent vocabulary. Follow both references and newer citing work; examine artifacts and production implementations, not titles alone. Record version/date, assumptions, closest overlap and the specific difference. Scope the search and keep unresolved leads visible.

A comparison matrix should distinguish semantics, resource budgets, maintenance work, failure model, data layout and workload. Novelty may lie in an explanation or rigorous characterization rather than a new algorithm. Merely combining familiar components or tuning one parameter is not automatically a research contribution.

## 3 Separate proof, model and test

For a protocol, define state, transitions, failures, message behavior and membership. State safety and liveness separately; liveness may require fairness, eventual communication or sufficient resources. A proof is conditional on its model. A finite model checker explores its modeled state space, not every possible implementation. Fuzzing and fault injection add implementation evidence but do not replace the argument.

For a performance model, name the resource and measurement boundary. Derive the predicted trend and break-even condition. Check units and limiting cases. A conjecture or heuristic must not be introduced as a universal lower bound.

## 4 Compare equal contracts before comparing speed

Match durable acknowledgment, isolation, data-loss policy, replication, retention, memory and space limits. If the new system trades a guarantee for performance, make that a visible axis rather than a hidden advantage.

Use a simple baseline, a strong tuned baseline, and the closest prior mechanism where feasible. Explain versions, options and tuning effort. If an artifact cannot run, record the limitation and narrow the conclusion; do not fabricate a substitute result.

Ablations remove or replace one proposed component to test its causal role. Include where the mechanism loses and compare against a simpler explanation for its gains.

## 5 Measure complete lifecycles

Include initialization, cache warmup, sustained operation, maintenance debt, failure, recovery and catch-up when relevant. Record offered load, successful goodput, rejections, errors, retries and acknowledged loss. Attribute asynchronous work rather than ending the experiment before it runs.

Vary dataset/working-set size, skew, value size, cardinality/churn, update/delete mix, lateness, snapshot age, outage duration and resource envelope. Use an explicit plan that prioritizes the variables most likely to disprove the claim. Exhaustively combining every knob is not necessarily informative.

One machine cannot establish all distributed scaling behavior; a simulated network cannot establish a real device's energy or persistence properties. Explain which conclusions transfer and which remain hypotheses.

## 6 Make uncertainty visible

Use independent repeated runs where meaningful, preserve seeds and configuration, randomize/block execution order when environmental drift could bias comparisons, and report dispersion or intervals appropriate to the design. Many requests in one run are not automatically many independent experimental replications.

Explain warmup and exclusion rules before selecting favorable windows. High percentiles need sufficient observations and a stated estimator; do not silently pool incompatible runs. Investigate failures and outliers rather than removing them to improve the chart.

Statistical differences do not by themselves establish a mechanism, and a tiny repeatable gain may not justify architectural complexity. Report effect size, cost and applicability alongside uncertainty.

## 7 Build the artifact alongside the paper

Preserve exact revisions, configuration, workload generators, seeds, raw measurements, analysis scripts, figure inputs, expected outputs, hardware requirements and known failure modes. Separate quick checks from expensive full reproduction. Another user should be able to find which artifact supports each major claim.

FAST's [artifact criteria](https://www.usenix.org/conference/fast27/call-for-artifacts) emphasize consistency with the paper, completeness, documentation and reuse. Its artifact process is distinct from paper acceptance. Do not claim an official badge for a locally rerun result.

The [PVLDB Volume 20 guidelines](https://www.vldb.org/2027/submission-guidelines.html) distinguish research categories and require EA&B papers to provide a reproducibility package at initial submission. Treat this as a dated category-specific example, not a timeless rule for every venue. Recheck the target's current policies when a paper is ready; this program does not select a deadline.

## 8 Write an argument that a skeptical reader can inspect

A useful paper progression is: important problem; inadequacy of existing approaches; key insight; mechanism/model; correctness and assumptions; evaluation; limitations; related work; conclusion tied to evidence. The contribution determines the precise organization.

For each figure, state the question, setup, observation and implication. The caption and surrounding text should explain units, workloads, error/uncertainty treatment and where the mechanism fails. Distinguish reported results from extrapolated outcomes.

Experience and measurement papers need a generalizable insight and rigorous evidence; a deployment narrative alone may not suffice. A survey needs a defensible scope/taxonomy and accurate synthesis. A vision paper must not present an unbuilt system as evaluated research. Check the actual venue/category expectations.

## 9 Review before submission

Prepare a skeptical review of your own work: strongest contribution, closest prior work, biggest correctness doubt, strongest alternative explanation, missing experiment and claims that exceed evidence. Have a technically qualified independent reader critique the argument when possible.

Respond by changing the claim, model or experiment as warranted. Rejection is information to analyze, not automatic proof the work is wrong or right. Distinguish research readiness from formatting readiness and from acceptance probability.

## Reusable paper brief

Before a substantial implementation, write one page containing the question, importance, closest work, proposed insight, assumptions, correctness obligation, planned figures, baselines, likely negative cases and early-stop criterion. Revise it after initial evidence. This is a research planning artifact, not permission to claim novelty or success.
