package com.greedybreaker.repository;

import com.greedybreaker.model.Experiment;
import com.greedybreaker.model.enums.ExperimentStatus;
import com.greedybreaker.model.enums.ProblemType;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface ExperimentRepository extends JpaRepository<Experiment, String> {
    Page<Experiment> findByProblemType(ProblemType problemType, Pageable pageable);
    Page<Experiment> findByStatus(ExperimentStatus status, Pageable pageable);
    Page<Experiment> findByProblemTypeAndStatus(ProblemType problemType, ExperimentStatus status, Pageable pageable);
    List<Experiment> findByParentExperimentId(String parentExperimentId);
}
