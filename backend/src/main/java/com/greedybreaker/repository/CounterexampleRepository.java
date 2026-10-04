package com.greedybreaker.repository;

import com.greedybreaker.model.Counterexample;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface CounterexampleRepository extends JpaRepository<Counterexample, String> {
    Optional<Counterexample> findByExecutionId(String executionId);
}
