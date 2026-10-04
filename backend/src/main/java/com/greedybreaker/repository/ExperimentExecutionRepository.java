package com.greedybreaker.repository;

import com.greedybreaker.model.ExperimentExecution;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;
import java.util.Optional;

@Repository
public interface ExperimentExecutionRepository extends JpaRepository<ExperimentExecution, String> {
    List<ExperimentExecution> findByExperimentIdOrderByStartedAtDesc(String experimentId);
    Optional<ExperimentExecution> findFirstByExperimentIdOrderByStartedAtDesc(String experimentId);
}
