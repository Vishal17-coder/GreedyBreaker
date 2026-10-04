package com.greedybreaker.service;

import com.greedybreaker.dto.response.ProblemMetadataDTO;
import com.greedybreaker.model.enums.ProblemType;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

@Service
public class ProblemRegistryService {

    private final List<ProblemMetadataDTO> problems = new ArrayList<>();

    public ProblemRegistryService() {
        initProblems();
    }

    private void initProblems() {
        // 1. Coin Change
        problems.add(new ProblemMetadataDTO(
                ProblemType.COIN_CHANGE.getId(),
                "Coin Change (Fewest Coins)",
                "Find the minimum number of coins to make change for a target value.",
                "MINIMIZE",
                List.of(
                        new ProblemMetadataDTO.StrategyInfo("largest_denomination_first", "Largest Denomination First", true)
                ),
                List.of(
                        new ProblemMetadataDTO.OracleInfo("dp_exact", "Dynamic Programming Unbounded Knapsack O(K*T)", "EXACT", true),
                        new ProblemMetadataDTO.OracleInfo("bfs_exact", "Breadth-First Search Exact Shortest Path", "EXACT", false)
                )
        ));

        // 2. 0/1 Knapsack
        problems.add(new ProblemMetadataDTO(
                ProblemType.KNAPSACK_01.getId(),
                "0/1 Knapsack Problem",
                "Maximize total value of items placed in knapsack without exceeding capacity.",
                "MAXIMIZE",
                List.of(
                        new ProblemMetadataDTO.StrategyInfo("value_density_descending", "Value Density Descending (v_i / w_i)", true),
                        new ProblemMetadataDTO.StrategyInfo("max_value_first", "Max Value First", false),
                        new ProblemMetadataDTO.StrategyInfo("min_weight_first", "Min Weight First", false)
                ),
                List.of(
                        new ProblemMetadataDTO.OracleInfo("dp_exact", "Dynamic Programming 2D Table O(N*W)", "EXACT", true),
                        new ProblemMetadataDTO.OracleInfo("exhaustive_exact", "Exhaustive Subset Search O(2^N)", "EXACT", false)
                )
        ));

        // 3. Activity Selection
        problems.add(new ProblemMetadataDTO(
                ProblemType.ACTIVITY_SELECTION.getId(),
                "Activity Selection Problem",
                "Maximize the count of non-overlapping activities scheduled in a single room.",
                "MAXIMIZE",
                List.of(
                        new ProblemMetadataDTO.StrategyInfo("earliest_finish_time", "Earliest Finish Time (Optimal Heuristic)", true),
                        new ProblemMetadataDTO.StrategyInfo("shortest_interval", "Shortest Interval Duration (Breakable Heuristic)", false)
                ),
                List.of(
                        new ProblemMetadataDTO.OracleInfo("dp_exact", "Interval Scheduling DP O(N^2)", "EXACT", true)
                )
        ));

        // 4. Fractional Knapsack
        problems.add(new ProblemMetadataDTO(
                ProblemType.FRACTIONAL_KNAPSACK.getId(),
                "Fractional Knapsack Problem",
                "Maximize value where arbitrary fractions of items may be selected.",
                "MAXIMIZE",
                List.of(
                        new ProblemMetadataDTO.StrategyInfo("value_density_descending", "Value Density Descending (Optimal Heuristic)", true),
                        new ProblemMetadataDTO.StrategyInfo("min_weight_first", "Min Weight First (Breakable Heuristic)", false)
                ),
                List.of(
                        new ProblemMetadataDTO.OracleInfo("continuous_lp_exact", "Continuous Linear Programming Relaxation Optimum", "EXACT", true)
                )
        ));

        // 5. Huffman Coding
        problems.add(new ProblemMetadataDTO(
                ProblemType.HUFFMAN_CODING.getId(),
                "Optimal Prefix Code (Huffman Coding)",
                "Minimize expected code length for symbol transmission.",
                "MINIMIZE",
                List.of(
                        new ProblemMetadataDTO.StrategyInfo("standard_huffman", "Standard Two-Lowest Frequency Merge (Optimal Heuristic)", true),
                        new ProblemMetadataDTO.StrategyInfo("shannon_fano", "Shannon-Fano Top-Down Cumulative Split (Breakable Heuristic)", false)
                ),
                List.of(
                        new ProblemMetadataDTO.OracleInfo("priority_queue_exact", "Optimal Prefix Tree Ground Truth", "EXACT", true)
                )
        ));
    }

    public List<ProblemMetadataDTO> listAllProblems() {
        return problems;
    }

    public Optional<ProblemMetadataDTO> findProblemById(String problemId) {
        return problems.stream()
                .filter(p -> p.getId().equalsIgnoreCase(problemId))
                .findFirst();
    }
}
